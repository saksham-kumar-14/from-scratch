use std::sync::Arc;
use std::sync::Mutex;
use std::thread;
use std::time::Duration;

/*
 * ??how to share memory across multiple independent threads when you don't know which thread will die first?
 * 1. static - mutating this is highly restricted
 * 2. Intentional leak
 *      Box::leak tells compiler to strip away ownership rules basically static it is now
 * 3. Single threaded reference counting (Rc)
 *      - this is std::shared_ptr from cpp
 *      - It wraps data alongside a hidden integer counter. Rc conter increase on doing `clone()`. When an Rc goes out of scope
 *        it is dropped and counter decreases
 *      - flaw: if two threads drop their Rc at the same CPU cycle -> creates race condition.
 *              Rust catches this at compile time and forbits Rc from çrossing thread boundaries
 * 4. Atomic Reference Couting (Arc)
 *      - Similar to Rc but its counter uses atomic CPU instructions (hardware leve lock free operations)
 *      - atomic operations carry a slight overhead at CPU cache
 */

fn scoped_threads() {
    let my_data = vec![1, 2, 3];

    thread::scope(|s| {
        // We spawn a thread using the scope `s`, NOT `thread::spawn`
        s.spawn(|| {
            // We are safely BORROWING `my_data` from the parent stack frame!
            println!("Thread 1 sees: {:?}", my_data);
        });

        s.spawn(|| {
            println!("Thread 2 also sees: {:?}", my_data);
        });
    });

    // We still own `my_data` here! The threads didn't consume it.
    println!("Main thread still owns: {:?}", my_data);
}

fn is_it_safe_to_transfer_ownership_from_one_to_another_thead() {
    // string is sent
    let my_string = String::from("I am moving to a new thread!");

    thread::spawn(move || {
        println!("{}", my_string);
    })
    .join()
    .unwrap();

    // Rc is not send
    // let my_rc = Rc::new(10);

    // thread::spawn(move || {
    //     // error[E0277]: `Rc<i32>` cannot be sent between threads safely
    //     println!("Trying to read Rc: {}", my_rc);
    // });

    // Arc is sent
    let my_arc = Arc::new(10);
    let arc_clone = my_arc.clone(); // Increment the atomic counter to 2

    thread::spawn(move || {
        // Safe: Arc uses hardware-level atomic instructions for its counter.
        println!("Background thread sees: {}", arc_clone);
    })
    .join()
    .unwrap();
}

fn is_it_safe_to_share_ref_of_data_across_multiple_threads() {
    // i32 is sync
    let number = 42;
    thread::scope(|s| {
        // Both threads get a shared, read-only reference (&i32).
        // Since reading simultaneously is safe, `i32` is Sync.
        s.spawn(|| println!("Thread 1 reads: {}", &number));
        s.spawn(|| println!("Thread 2 reads: {}", &number));
    });

    // cell is not sync
    // let my_cell = Cell::new(10);
    // thread::scope(|s| {
    //     // error[E0277]: `Cell<i32>` cannot be shared between threads safely
    //     s.spawn(|| my_cell.set(20));
    //     s.spawn(|| my_cell.set(30));
    // });

    // mutex is sync
    let my_mutex = Mutex::new(10);

    thread::scope(|s| {
        s.spawn(|| {
            let mut guard = my_mutex.lock().unwrap();
            *guard = 20;
        });

        s.spawn(|| {
            let mut guard = my_mutex.lock().unwrap();
            *guard = 30;
        });
    });
}

/*
 * thread parking is great for one to one communication between threads
 * for m to n type of communications between threads use `Condvar`
 */
fn thread_parking() {
    println!("Main: Spawning worker thread...");

    let worker = thread::spawn(|| {
        println!("Worker: Starting heavy initialization (takes 2 seconds)...");
        // We simulate work so the Main thread has plenty of time to run ahead.
        thread::sleep(Duration::from_secs(2));

        println!("Worker: Initialization done. I will now call thread::park().");

        // Because the Main thread already left a token, this will not put
        // the thread to sleep. It instantly consumes the token and continues.
        thread::park();

        println!("Worker: I successfully bypassed sleep and finished the job!");
    });

    println!("Main: I want the worker to keep going immediately after initialization.");

    // We send the wakeup signal before the worker has called `park()`.
    worker.thread().unpark();
    println!("Main: Unpark token placed on the worker thread.");

    // Wait for the worker to finish
    worker.join().unwrap();
    println!("Main: Program complete.");
}

fn main() {
    scoped_threads();
    is_it_safe_to_transfer_ownership_from_one_to_another_thead();
    is_it_safe_to_share_ref_of_data_across_multiple_threads();
    thread_parking();
}
