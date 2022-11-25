fn main() {
    let path = std::env::args().nth(1).unwrap();
    println!("Deleting {}", path);
    std::fs::remove_dir_all(&path).unwrap();
}
