*COPYRIGHT RUSU BOGDAN, 312CA 2022-2023*
# HOMEWORK 2 - LOAD-BALANCER

The purpose of the homework is to implement a `system` in the C language that distributes the given data such that each `server` contains data distributed in a `consistent` manner. On each addition of a server the data is possibly shifted from their original server to the newly added one. Similarly, on serveral `removal` the data contained on the server to be removed is distributed to all the other servers that are still present.

* The `load balancer` is implemented as an entity that holds the `hash_ring`, an array that holds the replicas of each server (`3` in number), each replica having the hash_value of its index and the id of the server it corresponds to
* moreover, the load balancer holds and array of pointers to servers that are `allocated only when the user demands so`, contributing to the efficiency of the program
* on each server addition, the `hashtable` corresponding to the server is created and allocated and is used for faster `retrievals` and additions of new pairs `key, value`
* whenever a `new` server is allocated, we check whether it is possible for values stored in other servers to migrate towards the new server with the help of the `consistent hashing` algorithm which is based on a `binary-search in a sorted array`
* if a server is demanded to be `removed` all the pairs `key, value` are transfered onto already existing servers using the same `consisting hashing` algorithm
* the method used for the `removal` of the replicas belonging to the `hashring` is implemented by setting the values of the hash_values of the server to be removed to the `maximum value` of `unsigned int` so as to use the `qsort` function and `realloc` to remove the server replicas and keep the hashring sorted; below is the snippet of code I just explained
  
```c
for (int i = 0; i < ldb->n_servers; i++) {
    if (ldb->hash_ring[i].idx == server_id) {
        ldb->hash_ring[i].val = UINT_MAX;
    }
}

qsort(ldb->hash_ring, ldb->n_servers, sizeof(hash_ring_elem), cmp_hash_ring);
ldb->n_servers -= 3;

hash_ring_elem *tmp = realloc(
                ldb->hash_ring,
                ldb->n_servers * sizeof(hash_ring_elem));
DIE(tmp == NULL, "error reallocating\n");

ldb->hash_ring = tmp;
```

## Usage
```
#Input
add_server 45624

add_server 47838

add_server 4974

store "2227008d3af215fef3a827b5e377bd9c" 
"trustedsnap Solid Women's Tunic"

add_server 89305

retrieve "2227008d3af215fef3a827b5e377bd9c"

store "d60e567db00840c3ca46251f108cff1b" "EIRA Casual Printed Women's Kurti"

retrieve "2227008d3af215fef3a827b5e377bd9c"

remove_server 4974

add_server 25301

store "5834e87253f72c0063d2d9baca38d573" "SportsHouse makhan chor krishna Radium Showpiece  -  23 cm"

add_server 58225

retrieve "2227008d3af215fef3a827b5e377bd9c"
store "6634de7b39a5b0658c74ab098450096d" "D-Link 
DIR-816 Wireless AC750 Dual Band Router"

store "0c82b9c474b0917dd6eaaa77e19d753a" "FashionPedia Canvas Shoes, Sneakers"

retrieve "6634de7b39a5b0658c74ab098450096d"

store "e8c42ea6c7944c0d77beca6db7ac8623" "Grafion by Grafion - Comfort Feel Women's Full Coverage Bra"

retrieve "e8c42ea6c7944c0d77beca6db7ac8623"
```
```
#Output

Stored trustedsnap Solid Women's Tunic on server 47838.

Retrieved trustedsnap Solid Women's Tunic from server 47838.

Stored EIRA Casual Printed Women's Kurti on server 4974.

Retrieved trustedsnap Solid Women's Tunic from server 47838.

Stored SportsHouse makhan chor krishna Radium Showpiece  -  23 cm on server 25301.

Retrieved trustedsnap Solid Women's Tunic from server 47838.

Stored D-Link DIR-816 Wireless AC750 Dual Band Router on server 89305.

Stored FashionPedia Canvas Shoes, Sneakers on server 58225.

Retrieved D-Link DIR-816 Wireless AC750 Dual Band Router from server 89305.

Stored Grafion by Grafion - Comfort Feel Women's Full Coverage Bra on server 58225.

Retrieved Grafion by Grafion - Comfort Feel Women's Full Coverage Bra from server 58225.
```
