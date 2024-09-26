use std::env;

fn replace_symbols(string: String) -> String {
    let mut new_string = String::new();

    for c in string.chars() {
        match c {
            '&' => new_string.push_str("&amp;"),
            '>' => new_string.push_str("&gt;"),
            '<' => new_string.push_str("&lt;"),
            _ => new_string.push(c),
        }
    }

    return new_string
}

fn main() {
    let args: Vec<String> = env::args().collect();
    let new_string = replace_symbols(args.get(1).unwrap().to_string());
    println!("{}", new_string);
}
