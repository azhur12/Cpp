#include <iostream>
#include "Ring_Buffer.h"

using namespace std;
int main() {
    RingBuffer<int> ring_buffer(5);

    for (int i = 0; i < 5; i++) {
        ring_buffer.push_front(i);
        ring_buffer.push_back(i);
    }

    for (int i = 0; i < ring_buffer.get_size(); i++) {
        cout << ring_buffer[i] << " ";
    }
    cout << endl;

    ring_buffer.pop_back();
    ring_buffer.pop_front();

    for (int i = 0; i < ring_buffer.get_size(); i++) {
        cout << ring_buffer[i] << " ";
    }

    return 0;
}
