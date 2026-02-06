use logos::Logos;

#[derive(Logos, Debug, PartialEq)]
#[logos(skip r"[ \t\n\f]+")]

pub enum Token {
    // Literals / variables
    #[regex(r"[0-9]+(\.[0-9]+)?", |lex| lex.slice().parse().ok())]
    Number(f64),
    #[regex(r#""([^"\\]|\\t|\\u|\\n|\\")*""#, |lex| lex.slice().to_string())]
    String(String),
    #[regex(r"[a-zA-Z_][a-zA-Z0-9_]*", |lex| lex.slice().to_string())]
    Variable(String),
    #[token("IsTheManWhoSoldTheWorldTo")]
    Assign,

    // print
    #[token("PrintAsYouAre")]
    Print,

    // function
    #[token("MilkIt")]
    Fn,

    // return
    #[token("Pissing")]
    Pissing,

    // while loop
    #[token("DrainYou")]
    While,

    // for loop
    #[token("Sifting")]
    For,

    // break
    #[token("KurtCobain")]
    Break,

    // if
    #[token("Polly")]
    If,
    // else if
    #[token("Prolly")]
    ElseIf,
    // else
    #[token("OhMe")]
    Else,

    // Force Exit
    #[token("KurtCobainWithGun")]
    Exit,

    // Start and End braces
    #[token("(")]
    LeftParen,
    #[token(")")]
    RightParen,
    #[token("{")]
    LeftCurly,
    #[token("}")]
    RightCurly,
    #[token("[")]
    LeftBracket,
    #[token("]")]
    RightBracket,

    // Operators
    #[token("+")]
    Add,
    #[token("-")]
    Substract,
    #[token("*")]
    Multiply,
    #[token("/")]
    Divide,
    #[token("%")]
    Modulus,

    // Comparators
    #[token("==")]
    Equals,
    #[token("<>")]
    NotEquals,
    #[token("<")]
    LessThan,
    #[token(">")]
    GreaterThan,
    #[token("<=")]
    LessThanEquals,
    #[token(">=")]
    GreaterThanEquals,

    // Booleans
    #[token("KurtIsGrudge")]
    True,
    #[token("KurtIsAvg")]
    False,

    // Others
    #[token(",")]
    Comma,
    #[token(";")]
    SemiColon,
}
