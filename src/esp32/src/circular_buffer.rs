use anyhow::Result;
use std::sync::{Arc, Mutex};

pub struct CircularBuffer {
    buffer: Vec<i32>,
    head: usize,
    size: usize,
}

impl CircularBuffer {
    pub fn new(capacity: usize) -> Self {
        Self {
            buffer: vec![0; capacity],
            head: 0,
            size: 0,
        }
    }

    pub fn push(&mut self, data: &[i32]) {
        for &sample in data {
            self.buffer[self.head] = sample;
            self.head = (self.head + 1) % self.buffer.len();
            if self.size < self.buffer.len() {
                self.size += 1;
            }
        }
    }

    pub fn read_latest(&self, out: &mut [i32]) {
        let capacity = self.buffer.len();
        let len = out.len().min(self.size);
        
        for i in 0..len {
            let idx = (self.head + capacity - len + i) % capacity;
            out[i] = self.buffer[idx];
        }
    }

    pub fn size(&self) -> usize {
        self.size
    }
}
