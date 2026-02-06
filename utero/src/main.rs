use std::env;
use std::fs;
use std::path::Path;
use std::process;

use logos::Logos;

mod ast;
mod eval;
mod lexer;
mod parser;

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() < 2 {
        eprintln!("Usage: {} <filename>", args[0]);
        process::exit(1);
    }
    let filename = &args[1];
    let path = Path::new(filename);

    match path.extension().and_then(|s| s.to_str()) {
        Some("utr") => {}
        Some(other) => {
            eprintln!("Error: File must end in .utero, but found .{}", other);
            process::exit(1);
        }
        None => {
            eprintln!("Error: File has no extension. Expected .utero");
            process::exit(1);
        }
    }

    let code = match fs::read_to_string(filename) {
        Err(error) => {
            eprintln!("**Error reading \"{}\": {}", filename, error);
            process::exit(1);
        }
        Ok(code) => code,
    };

    // lexer
    let tokens: Vec<lexer::Token> = lexer::Token::lexer(&code).filter_map(Result::ok).collect();
    println!("Tokens : {:?}", tokens);
}
