# url_match_trie_tree
## url match tool based trie and regex

limitation:
- support trie character: string.digits string.ascii_letters '/' '-', escape character: '<' '>'
- it would be better that the url-match-string are original split by '/'. like **'/post/page/<\d+>'**, which won't cause a ambiguity match. if u insert **'/abc/<\d+>1'** and search value by **'/abc/1231'**, in this case the return value is url-match-string **/abc/<\d+>** inserted, which may unexpected by u.


you can add more supported character by modify variable alphabet and char_map

python usage:
```
    >>> initial_char_map()
    >>> tree = TrieRgxTree()
    >>> tree.insert(r'12def/ghi', 20)
    >>> tree.insert(r'12<\d+>', 10)
    >>> assert tree.search('12456') == 10
    >>> assert tree.search('12def/ghi') == 20
```

c usage:
```c
int trigx_test() {
    TrigxNode *node = create_trigx_node();
    trigx_insert(node, "123def", 6, 20);
    trigx_insert(node, "12<\\d+>abc", 10, 10);
    int i = trigx_search(node, "123def", 6);
    int j = trigx_search(node, "123abc", 6);
    printf("result of search %d, %d\n", i, j);
    assert(i == 20);
    return 1;
}
```

lua ffi usage:
u should check LD_LIBARARY_PATH variable has been set appropriate
```lua

-- file: trigx_tree.lua 
-- command: luajit -i trigx_tree.lua

local ffi = require "ffi"
local _M = {}

ffi.cdef [=[
    typedef struct _TrigxNode {
        int val;
        struct _TrigxNode *char_nodes[64];
        struct _TrigxNode *rgx_next;
        int rgx_raw_len;
        void *re;
        char *rgx;
    } TrigxNode;
    TrigxNode *create_trigx_node();
    void trigx_insert(TrigxNode *root, const char *word, int len_word, int val);
    int trigx_search(TrigxNode *root, const char *word, int len_word);
    void trigx_free(TrigxNode *root);
   ]=];

_M.C = ffi.load("trigx_tree")

local tree = _M.C.create_trigx_node()
_M.C.trigx_insert(tree, "123def", 6, 20)
_M.C.trigx_insert(tree, "page-<\\d+>/234", 10, 30)
local val = _M.C.trigx_search(tree, "123def", 6)
local val1 = _M.C.trigx_search(tree, "page-123/234", 8)
print(val)
print(val1)
```
