{ pkgs ? import <nixpkgs> {
  overlays = [
    (final: prev: {
      gnuplot = prev.gnuplot.overrideAttrs (old: {
        version = "6.0.0";

        src = prev.fetchurl {
          url = "mirror://sourceforge/gnuplot/gnuplot-6.0.0.tar.gz";
          sha256 = "sha256-Y1oo8Jk/arDRF54HKtObgTnQf1Ejf4Qdk8bC/0sXWOw=";
        };
      });
    })
  ];
} }:

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
