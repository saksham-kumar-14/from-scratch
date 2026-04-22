{
  description = "yo this opengl ok";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs =
    {
      self,
      nixpkgs,
      flake-utils,
    }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in
      {
        devShells.default = pkgs.mkShell {
          packages =
            with pkgs;
            [
              cmake
              ninja
              pkg-config
              glfw
              glm
            ]
            ++ pkgs.lib.optionals pkgs.stdenv.isDarwin [
              apple-sdk
            ];

          shellHook = ''
            echo "yo this opengl dev env ok"
          '';
        };
      }
    );
}
