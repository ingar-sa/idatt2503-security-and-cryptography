#ifndef SDB_H_
#define SDB_H_

// NOLINTBEGIN(misc-definitions-in-headers)

#include <assert.h>
#include <errno.h>
#include <float.h>
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#if !defined(__cplusplus)
#include <stdbool.h>
#endif // C/C++

////////////////////////////////////////
//              DEFINES               //
////////////////////////////////////////

#define SDB_BEGIN_EXTERN_C                                                                         \
    extern "C"                                                                                     \
    {
#define SDB_END_EXTERN_C }

// NOTE(ingar): Prevents the formatter from indenting everything inside the
// extern "C" block
#if defined(__cplusplus)
SDB_BEGIN_EXTERN_C
#endif

#define sdb_internal static
#define sdb_persist  static
#define sdb_global   static

// TODO(ingar): Add support for custom errno codes
/* 0 is defined as success; negative errno code otherwise. */
typedef int_least32_t sdb_errno;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

// NOTE(ingar): DO NOT USE THESE OUTSIDE OF THIS FILE! Since we are working with
// embedded systems floats and doubles may not necessarily be 32-bit and 64-bit
// respectively
typedef float  f32;
typedef double f64;

#define SDB_EXPAND(x)       x
#define SDB__STRINGIFY__(x) #x
#define SDB_STRINGIFY(x)    SDB__STRINGIFY__(x)

#define SDB__CONCAT2__(x, y) x##y
#define SDB_CONCAT2(x, y)    SDB__CONCAT2__(x, y)

#define SDB__CONCAT3__(x, y, z) x##y##z
#define SDB_CONCAT3(x, y, z)    SDB__CONCAT3__(x, y, z)

#define SdbKiloByte(Number) (Number * 1000ULL)
#define SdbMegaByte(Number) (SdbKiloByte(Number) * 1000ULL)
#define SdbGigaByte(Number) (SdbMegaByte(Number) * 1000ULL)
#define SdbTeraByte(Number) (SdbGigaByte(Number) * 1000ULL)

#define SdbKibiByte(Number) (Number * 1024ULL)
#define SdbMebiByte(Number) (SdbKibiByte(Number) * 1024ULL)
#define SdbGibiByte(Number) (SdbMebiByte(Number) * 1024ULL)
#define SdbTebiByte(Number) (SdbGibiByte(Number) * 1024ULL)

#define SdbArrayLen(Array) (sizeof(Array) / sizeof(Array[0]))

#define SdbMax(a, b) ((a > b) ? a : b)
#define SdbMin(a, b) ((a < b) ? a : b)

////////////////////////////////////////
//               MISC                 //
////////////////////////////////////////

bool
SdbDoubleEpsilonCompare(const double A, const double B)
{
    double GreatestValue = (fabs(A) < fabs(B)) ? fabs(B) : fabs(A);
    return fabs(A - B) <= (GreatestValue * DBL_EPSILON);
}

u64
SdbDoubleSignBit(double F)
{
    u64  Mask = 1ULL << 63;
    u64 *Comp = (u64 *)&F;

    return (*Comp) & Mask;
}

double
SdbRadiansFromDegrees(double Degrees)
{
    double Radians = 0.01745329251994329577f * Degrees;
    return Radians;
}

////////////////////////////////////////
//              LOGGING               //
////////////////////////////////////////
/* Based on the logging frontend I wrote for oec */

#if !defined(SDB_LOG_BUF_SIZE)
#define SDB_LOG_BUF_SIZE 1024
#endif

#if SDB_LOG_BUF_SIZE < 128
#error "SDB_LOG_BUF_SIZE must be greater than or equal to 128!"
#endif

typedef struct sdb__log_module__
{
    const char *Name;
    u64         BufferSize;
    char       *Buffer;
} sdb__log_module__;

i64
Sdb__WriteLog__(sdb__log_module__ *Module, const char *LogLevel, va_list VaArgs)
{
    time_t     PosixTime;
    struct tm *TimeInfo;

    if((time_t)(-1) == time(&PosixTime)) {
        return -errno;
    }

    TimeInfo = localtime(&PosixTime);
    if(NULL == TimeInfo) {
        return -errno;
    }

    u64 CharsWritten    = 0;
    u64 BufferRemaining = Module->BufferSize;

    int FormatRet = strftime(Module->Buffer, BufferRemaining, "%T: ", TimeInfo);
    if(0 == FormatRet) {
        // NOTE(ingar): Since the buffer size is at least 128, this should never
        // happen
        assert(FormatRet);
        return -ENOMEM;
    }

    CharsWritten = FormatRet;
    BufferRemaining -= FormatRet;

    FormatRet = snprintf(Module->Buffer + CharsWritten, BufferRemaining, "%s: %s: ", Module->Name,
                         LogLevel);
    if(FormatRet < 0) {
        return -errno;
    } else if((u64)FormatRet >= BufferRemaining) {
        // NOTE(ingar): If the log module name is so long that it does not fit in
        // 128 bytes - the time stamp, it should be changed
        assert(FormatRet);
        return -ENOMEM;
    }

    CharsWritten += FormatRet;
    BufferRemaining -= FormatRet;

    const char *FormatString = va_arg(VaArgs, const char *);
    FormatRet = vsnprintf(Module->Buffer + CharsWritten, BufferRemaining, FormatString, VaArgs);

    if(FormatRet < 0) {
        return -errno;
    } else if((u64)FormatRet >= BufferRemaining) {
        (void)memset(Module->Buffer + CharsWritten, 0, BufferRemaining);
        FormatRet = snprintf(Module->Buffer + CharsWritten, BufferRemaining, "%s",
                             "Message dropped; too big");
        if(FormatRet < 0) {
            return -errno;
        } else if((u64)FormatRet >= BufferRemaining) {
            assert(FormatRet);
            return -ENOMEM;
        }
    }

    CharsWritten += FormatRet;
    Module->Buffer[CharsWritten++] = '\n';

    int OutFd;
    if('E' == LogLevel[0]) {
        OutFd = STDERR_FILENO;
    } else {
        OutFd = STDOUT_FILENO;
    }

    if((ssize_t)(-1) == write(OutFd, Module->Buffer, CharsWritten)) {
        return -errno;
    }

    return 0;
}

sdb_errno
Sdb__WriteLogIntermediate__(sdb__log_module__ *Module, const char *LogLevel, ...)
{
    va_list VaArgs;
    va_start(VaArgs, LogLevel);
    sdb_errno Ret = Sdb__WriteLog__(Module, LogLevel, VaArgs);
    va_end(VaArgs);
    return Ret;
}

sdb_errno
Sdb__WriteLogNoModule__(const char *LogLevel, const char *FunctionName, ...)
{
    char             *Buffer = (char *)calloc(SDB_LOG_BUF_SIZE, sizeof(char));
    sdb__log_module__ Module = {
        .Name       = FunctionName,
        .BufferSize = SDB_LOG_BUF_SIZE,
        .Buffer     = Buffer,
    };

    va_list VaArgs;
    va_start(VaArgs, FunctionName);
    sdb_errno Ret = Sdb__WriteLog__(&Module, LogLevel, VaArgs);
    va_end(VaArgs);

    free(Buffer);

    return Ret;
}

// #if !defined(SDB_LOG_LEVEL)
// #define SDB_LOG_LEVEL 3
// #endif

extern u64 SBD_LOG_LEVEL;

#define SDB_LOG_LEVEL_NONE (1U)
#define SDB_LOG_LEVEL_ERR  (2U)
#define SDB_LOG_LEVEL_INF  (3U)
#define SDB_LOG_LEVEL_DBG  (4U)

#define SDB__LOG_LEVEL_CHECK__(level) (SDB_LOG_LEVEL >= SDB_LOG_LEVEL_##level)

#define SDB_LOG_REGISTER(module_name)                                                              \
    sdb_global char SDB_CONCAT3(Sdb__LogModule, module_name, Buffer__)[SDB_LOG_BUF_SIZE];          \
    sdb_global sdb__log_module__ SDB_CONCAT3(Sdb__LogModule, module_name, __)                      \
        __attribute__((used))                                                                      \
        = { .Name       = #module_name,                                                            \
            .BufferSize = SDB_LOG_BUF_SIZE,                                                        \
            .Buffer     = SDB_CONCAT3(Sdb__LogModule, module_name, Buffer__) };                        \
    sdb_global sdb__log_module__ *Sdb__LogInstance__ __attribute__((used))                         \
    = &SDB_CONCAT3(Sdb__LogModule, module_name, __)

#define SDB_LOG_DECLARE_EXTERN(name)                                                               \
    extern sdb__log_module__      SDB_CONCAT3(Sdb__LogModule, name, __);                           \
    sdb_global sdb__log_module__ *Sdb__LogInstance__ = &SDB_CONCAT3(Sdb__LogModule, name, __)

#define SDB_LOG_DECLARE_SAME_TU extern struct sdb__log_module__ *Sdb__LogInsta

#define SDB__LOG__(log_level, ...)                                                                 \
    do {                                                                                           \
        if(SDB__LOG_LEVEL_CHECK__(log_level)) {                                                    \
            Sdb__WriteLogIntermediate__(Sdb__LogInstance__, SDB_STRINGIFY(log_level),              \
                                        __VA_ARGS__);                                              \
        }                                                                                          \
    } while(0)

#define SDB__LOG_NO_MODULE__(log_level, ...)                                                       \
    do {                                                                                           \
        if(SDB__LOG_LEVEL_CHECK__(log_level)) {                                                    \
            sdb_errno LogRet                                                                       \
                = Sdb__WriteLogNoModule__(SDB_STRINGIFY(log_level), __func__, __VA_ARGS__);        \
            assert(LogRet >= 0);                                                                   \
        }                                                                                          \
    } while(0)

#define SdbLogDebug(...)   SDB__LOG__(DBG, __VA_ARGS__)
#define SdbLogInfo(...)    SDB__LOG__(INF, __VA_ARGS__)
#define SdbLogWarning(...) SDB__LOG__(WRN, __VA_ARGS__)
#define SdbLogError(...)   SDB__LOG__(ERR, __VA_ARGS__)

// WARN: Uses calloc!
#define SdbLogDebugNoModule(...)   SDB__LOG_NO_MODULE__(DBG, __VA_ARGS__)
#define SdbLogInfoNoModule(...)    SDB__LOG_NO_MODULE__(INF, __VA_ARGS__)
#define SdbLogWarningNoModule(...) SDB__LOG_NO_MODULE__(WRN, __VA_ARGS__)
#define SdbLogErrorNoModule(...)   SDB__LOG_NO_MODULE__(ERR, __VA_ARGS__)

#define SdbAssert(condition)                                                                       \
    do {                                                                                           \
        if(!(condition)) {                                                                         \
            SdbLogError("Assertion failed: " SDB_STRINGIFY(condition));                            \
            assert(condition);                                                                     \
        }                                                                                          \
    } while(0)

////////////////////////////////////////
//             ALLOCATORS             //
////////////////////////////////////////

void
SdbMemZero(void *Mem, u64 Size)
{
    for(u64 i = 0; i < Size; ++i) {
        ((u8 *)Mem)[i] = 0;
    }
}

void /* From https://github.com/BLAKE2/BLAKE2/blob/master/ref/blake2-impl.h */
SdbMemZeroSecure(void *Mem, u64 Size)
{
    static void *(*const volatile memset_v)(void *, int, u64) = &memset;
    (void)memset_v(Mem, 0, Size);
}

#define SdbMemZeroStruct(struct)       SdbMemZero(struct, sizeof(*struct))
#define SdbMemZeroStructSecure(struct) SdbMemZeroSecure(struct, sizeof(*struct))

typedef struct sdb_arena
{
    u64 Cur;
    u64 Cap;
    // u64 Save; /* DOES NOT WORK!!! Makes it easier to use the arena as a stack
    // */
    u8 *Mem; /* If it's last, the arena's memory can be contiguous with the struct
                itself */
} sdb_arena;

typedef struct sdb_slice
{
    u64 Len; /* Not size_t since I want the member size to be constant */
    u64 ESize;
    u8 *Mem;
} sdb_slice;

void
SdbArenaInit(sdb_arena *Arena, u8 *Mem, u64 Size)
{
    Arena->Cur = 0;
    Arena->Cap = Size;
    // Arena->Save = 0;
    Arena->Mem = Mem;
}

sdb_arena *
SdbArenaCreateContiguous(u8 *Mem, u64 Size)
{
    if(Size < (sizeof(sdb_arena) + 1)) {
        return NULL;
    }

    sdb_arena *Arena = (sdb_arena *)Mem;
    Arena->Cap       = Size - sizeof(sdb_arena);
    Arena->Cur       = 0;
    // Arena->Save      = 0;
    Arena->Mem = Mem + sizeof(sdb_arena);

    return Arena;
}

#if 0
u64
SdbArenaF5(sdb_arena *Arena)
{
    Arena->Save = Arena->Cur;
    return Arena->Save;
}

u64
SdbArenaF9(sdb_arena *Arena)
{
    Arena->Cur  = Arena->Save;
    Arena->Save = 0;

    return Arena->Cur;
}
#endif

// TODO(ingar): Create aligning pushes
void *
SdbArenaPush(sdb_arena *Arena, u64 Size)
{
    if((Arena->Cur + Size) < Arena->Cap) {
        u8 *AllocedMem = Arena->Mem + Arena->Cur;
        Arena->Cur += Size;

        return (void *)AllocedMem;
    }

    return NULL;
}

void *
SdbArenaPushZero(sdb_arena *Arena, u64 Size)
{
    if((Arena->Cur + Size) < Arena->Cap) {
        u8 *AllocedMem = Arena->Mem + Arena->Cur;
        Arena->Cur += Size;
        SdbMemZero(AllocedMem, Size);

        return (void *)AllocedMem;
    }

    return NULL;
}

void
SdbArenaPop(sdb_arena *Arena, u64 Size)
{
    assert(Arena->Cur >= Size);
    Arena->Cur -= Size;
}

u64
SdbArenaGetPos(sdb_arena *Arena)
{
    u64 Pos = Arena->Cur;
    return Pos;
}

void
SdbArenaSeek(sdb_arena *Arena, u64 Pos)
{
    assert(Pos <= Arena->Cap);
    Arena->Cur = Pos;
}

void
SdbArenaClear(sdb_arena *Arena)
{
    Arena->Cur = 0;
}

void
SdbArenaClearZero(sdb_arena *Arena)
{
    SdbMemZero(Arena->Mem, Arena->Cap);
    Arena->Cur = 0;
}

void
SdbArrayShift(void *Mem, u64 From, u64 To, u64 Count, u64 ElementSize)
{
    if(From != To) {
        u8 *Array = (u8 *)Mem;
        u8 *Src   = Array + (From * ElementSize);
        u8 *Dst   = Array + (To * ElementSize);

        u64 NumElements = (Count > From) ? (Count - From) : 0;

        if(NumElements > 0) {
            u64 NBytes = NumElements * ElementSize;
            memmove(Dst, Src, NBytes);
        }
    }
}

#define SdbArrayDeleteAndShift(mem, i, count, esize) SdbArrayShift(mem, (i + 1), i, count, esize)

#define SdbPushArray(arena, type, count)     (type *)SdbArenaPush(arena, sizeof(type) * (count))
#define SdbPushArrayZero(arena, type, count) (type *)SdbArenaPushZero(arena, sizeof(type) * (count))

#define SdbPushStruct(arena, type)     SdbPushArray(arena, type, 1)
#define SdbPushStructZero(arena, type) SdbPushArrayZero(arena, type, 1)

#define SDB_DEFINE_POOL_ALLOCATOR(type_name, func_name)                                            \
    typedef struct type_name##_Pool                                                                \
    {                                                                                              \
        sdb_arena *Arena;                                                                          \
        type_name *FirstFree;                                                                      \
    } type_name##_pool;                                                                            \
                                                                                                   \
    type_name *func_name##Alloc(type_name##_pool *Pool)                                            \
    {                                                                                              \
        type_name *Result = Pool->FirstFree;                                                       \
        if(Result) {                                                                               \
            Pool->FirstFree = Pool->FirstFree->Next;                                               \
            SdbMemZeroStruct(Result);                                                              \
        } else {                                                                                   \
            Result = SdbPushStructZero(Pool->Arena, type_name);                                    \
        }                                                                                          \
                                                                                                   \
        return Result;                                                                             \
    }                                                                                              \
                                                                                                   \
    void func_name##Release(type_name##_pool *Pool, type_name *Instance)                           \
    {                                                                                              \
        Instance->Next  = Pool->FirstFree;                                                         \
        Pool->FirstFree = Instance;                                                                \
    }

////////////////////////////////////////
//              STRINGS               //
////////////////////////////////////////

typedef struct sdb_string
{
    u64         Len; /* Does not include the null terminator*/
    const char *S;   /* Will always be null-terminated for simplicity */
} sdb_string;

void
SdbMemcpy(void *To, void *From, u64 Len)
{
    // NOTE(ingar): The compiler should replace this with memcpy if it's available
    for(u64 i = 0; i < Len; ++i) {
        ((u8 *)To)[i] = ((u8 *)From)[i];
    }
}

u64
SdbStrlen(const char *String)
{
    u64 Count = 0;
    while(*String++ != '\0') {
        ++Count;
    }

    return Count;
}

char *
SdbStrdup(char *String, sdb_arena *Arena)
{
    u64   StringLength = SdbStrlen(String);
    char *NewString    = SdbPushArrayZero(Arena, char, StringLength + 1);
    SdbMemcpy(NewString, String, StringLength);
    NewString[StringLength] = '\0';

    return NewString;
}

////////////////////////////////////////
//                RNG                 //
////////////////////////////////////////

u32 *
Sdb__GetPCGState__(void)
{
    sdb_persist uint32_t Sdb__PCGState = 0;
    return &Sdb__PCGState;
}

// Implementation of the PCG algorithm (https://www.pcg-random.org)
// It's the caller's responsibilites to have called SeedRandPCG before use
u32
SdbRandPCG(void)
{
    u32 State             = *Sdb__GetPCGState__();
    *Sdb__GetPCGState__() = State * 747796405u + 2891336453u;
    u32 Word              = ((State >> ((State >> 28u) + 4u)) ^ State) * 277803737u;
    return (Word >> 22u) ^ Word;
}

void
SdbSeedRandPCG(uint32_t Seed)
{
    *Sdb__GetPCGState__() = Seed;
}

/////////////////////////////////////////
//              FILE IO                //
/////////////////////////////////////////

typedef struct sdb_file_data
{
    u64 Size;
    u8 *Data;
} sdb_file_data;

sdb_file_data *
SdbLoadFileIntoMemory(const char *Filename, sdb_arena *Arena)
{
    FILE *fd = fopen(Filename, "rb");
    if(!fd) {
        return NULL;
    }

    if(fseek(fd, 0L, SEEK_END) != 0) {
        fclose(fd);
        return NULL;
    }

    u64 FileSize = (u64)ftell(fd);
    rewind(fd);

    sdb_file_data *FileData = SdbPushStructZero(Arena, sdb_file_data);
    FileData->Data          = SdbPushArray(Arena, u8, FileSize + 1);
    /*
    if(!FileData)
    {
        fprintf(stderr, "Could not allocate memory for file!\n");
        fclose(fd);
        return NULL;
    }
    */

    FileData->Size = FileSize;
    u64 BytesRead  = fread(FileData->Data, 1, FileSize, fd);
    if(BytesRead != FileSize) {
        fclose(fd);
        SdbArenaPop(Arena, sizeof(sdb_file_data) + FileData->Size + 1);
        return NULL;
    }

    fclose(fd);

    FileData->Data[FileData->Size] = '\0';
    return FileData;
}

bool
SdbWriteBufferToFile(void *Buffer, u64 ElementSize, u64 ElementCount, const char *Filename)
{
    FILE *fd = fopen(Filename, "wb");
    if(!fd) {
        fprintf(stderr, "Unable to open file %s!\n", Filename);
        return false;
    }

    bool WriteSuccessful = fwrite(Buffer, ElementSize, ElementCount, fd) == ElementCount;
    fclose(fd);
    return WriteSuccessful;
}

bool
SdbWrite_sdb_file_data_ToFile(sdb_file_data *FileData, const char *Filename)
{
    FILE *fd = fopen(Filename, "wb");
    if(!fd) {
        fprintf(stderr, "Failed to open file during file_data write!\n");
        return false;
    }

    bool WriteSuccessful
        = fwrite(FileData->Data, sizeof(uint8_t), FileData->Size, fd) == FileData->Size;
    fclose(fd);
    return WriteSuccessful;
}

////////////////////////////////////////
//             TOKENIZER              //
////////////////////////////////////////

// TODO(ingar): This is not a general purpose tokenizer,
// but it is an example of an implementation
typedef struct sdb_token
{
    char *Start;
    u64   Len;
} sdb_token;

sdb_token
SdbGetNextToken(char **Cursor)
{
    while('\t' != **Cursor) {
        (*Cursor)++;
    }

    (*Cursor)++; // Skips to start of hex number

    sdb_token Token;
    Token.Start = *Cursor;
    Token.Len   = 0;

    while('\n' != **Cursor && '\r' != **Cursor) {
        (*Cursor)++;
        ++Token.Len;
    }

    if('\0' != **Cursor) {
        **Cursor = '\0';
    }

    return Token;
}
#if defined(__cplusplus)
SDB_END_EXTERN_C
#endif

// NOLINTEND(misc-definitions-in-headers)

#endif // SDB_H_
