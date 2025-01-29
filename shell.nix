{ pkgs ? import <nixpkgs> { } }:

pkgs.mkShell {
  packages = with pkgs; [
    nixfmt-classic
    nil
    cmake
    ninja
    vcpkg
    pkg-config
    gnuplot
  ];

  shellHook = ''
    export VCPKG_ROOT="${pkgs.vcpkg}/share/vcpkg"
  '';
}
