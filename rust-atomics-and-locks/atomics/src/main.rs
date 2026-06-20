use std::{
    sync::atomic::{AtomicBool, Ordering::Relaxed},
    thread,
};

/*
   Notice that store takes a shared reference (&self), not an exclusive one (&mut self)
   Atomics use interior mutability, meaning you can modify them even when multiple threads hold shared references to them.
*/
fn main() {
    static STOP: AtomicBool = AtomicBool::new(false);

    // spawn a thread
    let bg_thread = thread::spawn(|| {
        while !STOP.load(Relaxed) {
            println!("Hey users, interact with this background thread!");
        }
    });

    // use the main thread to listen to user input
    for line in std::io::stdin().lines() {
        match line.unwrap().as_str() {
            "help" => println!("commands: help and stop"),
            "stop" => break,
            cmd => println!("unknown command: {cmd:?}"),
        }
    }

    // inform the background thread to stop
    STOP.store(true, Relaxed);
    // wait till the background task finishes
    bg_thread.join().unwrap();
}
