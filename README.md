# url_match_trie_tree
## url match tool based trie and regex

support trie character: string.digits string.ascii_letters '/' '-', escape character: '<' '>'

you can add more supported character by modify variable alphabet and char_map

usage:
```
    >>> initial_char_map()
    >>> tree = TrieRgxTree()
    >>> tree.insert(r'12def/ghi', 20)
    >>> tree.insert(r'12<\d+>', 10)
    >>> assert tree.search('12456') == 10
    >>> assert tree.search('12def/ghi') == 20
```
