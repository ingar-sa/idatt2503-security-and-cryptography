let
  pkgs = import <nixpkgs-unstable> {};
in pkgs.mkShell {
  packages = [
    (pkgs.python3.withPackages (python-pkgs: [
      python-pkgs.pwntools
    ]))
  ];
}
