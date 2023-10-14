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

## Personal comments
* As for some personal comments, I would like to appreciate the effort put into the making of the homework by the team, providing us with the possibily of implementing something that has quite some `practicability` into real-life `applications` and for the use of `data structures` in a not-so-abstract environment
* As I am always striving to do better with each homework, I would point out that my `modularization` could have been better in some cases but for the most part I am satisfied with how this homework turned out to be implemented
* I have kept the links from the `hashing methods` used for the `hash_table` implementation as they were taken from the laboratory, where the credits had been provided beforehand
