{ pkgs ? import <nixpkgs> {} }:
pkgs.mkShell {
  buildInputs = with pkgs; [
    gcc gnumake go-task
    pkg-config libx11.dev libxrandr.dev libxinerama.dev libxcursor.dev libxi.dev libGL.dev
  ];
}
