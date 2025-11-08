/* $Id$ */
#ifndef TABLE_INCLUDED
#define TABLE_INCLUDED
#define T Table_T
typedef struct T *T;

/// @brief Create a new hash table.
/// @param hint Initial size hint for the table.
/// @param cmp Comparison function for keys.
/// @param hash Hash function for keys.
/// @return Pointer to the new table.
extern T        Table_new       (int hint,
                                int cmp(const void *x, const void *y),
                                unsigned hash(const void *key));
/// @brief Free a hash table.
/// @param table Pointer to the table to free.
extern void     Table_free      (T *table);
/// @brief Get the number of key-value pairs in a hash table.
/// @param table Pointer to the table.
/// @return Number of key-value pairs in the table.
extern int      Table_length    (T table);
/// @brief Insert a key-value pair into a hash table.
/// @param table Pointer to the table.
/// @param key Pointer to the key.
/// @param value Pointer to the value.
/// @return Pointer to the previous value associated with the key, or NULL if the key was not found.
extern void     *Table_put      (T table, const void *key, void *value);
/// @brief Retrieve a value from a hash table.
/// @param table Pointer to the table.
/// @param key Pointer to the key.
/// @return Pointer to the value associated with the key, or NULL if the key was not found.
extern void     *Table_get      (T table, const void *key);
/// @brief Remove a key-value pair from a hash table.
/// @param table Pointer to the table.
/// @param key Pointer to the key.
/// @return Pointer to the value associated with the key, or NULL if the key was not found.
extern void     *Table_remove   (T table, const void *key);
/// @brief Apply a function to each key-value pair in a hash table.
/// @param table Pointer to the table.
/// @param apply Function to apply to each key-value pair.
/// @param cl Closure to pass to the apply function.
/// @return Pointer to the value associated with the key, or NULL if the key was not found.
extern void     Table_map      (T table,
                                void apply(const void *key, void **value, void *cl),
                                void *cl);
/// @brief Convert a hash table to an array.
/// @param table Pointer to the table.
/// @param length Pointer to an integer to store the length of the array.
/// @return Pointer to the array of key-value pairs, or NULL if the table is empty.
extern void     **Table_toArray  (T table, void *end);
#undef T
#endif