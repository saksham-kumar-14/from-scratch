type terminal = char
type non_terminal = string

(* Production rules *)
type production =
  | Binary of non_terminal * non_terminal * non_terminal
  | Unary of non_terminal * terminal

module non_terminal_set = Set.Make(String)
