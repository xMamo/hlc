#include "map.h"

#include <assert.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "allocator.h"
#include "comparator.h"
#include "layout.h"

typedef struct _Node {
  bool red;
  struct _Node* parent;
  struct _Node* left;
  struct _Node* right;
  char data[];
} _Node;

typedef struct _Node_layout {
  size_t size;
  size_t alignment;
  size_t key_offset;
  size_t value_offset;
} _Node_layout;

typedef struct _Map {
  _Node* root;
  size_t count;
  Layout key_layout;
  Layout value_layout;
  Comparator comparator;
  Allocator allocator;
  _Node_layout node_layout;
} _Map;

static inline void* _node_key(const _Node* node, const _Node_layout* node_layout) {
  return (char*)node + node_layout->key_offset;
}

static inline void* _node_value(const _Node* node, const _Node_layout* node_layout) {
  return (char*)node + node_layout->value_offset;
}

static inline bool _node_red(const _Node* node) {
  return node != NULL && node->red;
}

static inline bool _node_black(const _Node* node) {
  return node == NULL || !node->red;
}

static inline _Node** _node_rfp(const _Node* node, _Node* const* root_ref) {
  if (node->parent != NULL) {
    return node == node->parent->left ? &node->parent->left : &node->parent->right;
  } else {
    return (_Node**)root_ref;
  }
}

static void _node_color_flip(_Node* node) {
  node->left->red = !node->left->red;
  node->right->red = !node->right->red;
  node->red = !node->red;
}

//   x              y
//  / \            / \
// A   y    =>    x   C
//    / \        / \
//   B   C      A   B
static void _node_rotate_left(_Node** node_rfp) {
  _Node* x = *node_rfp;

  bool x_red = x->red;
  _Node* x_parent = x->parent;
  _Node* y = x->right;

  bool y_red = y->red;
  _Node* b = y->left;

  if (b != NULL) {
    b->parent = x;
  }

  x->red = y_red;
  x->parent = y;
  x->right = b;

  y->red = x_red;
  y->parent = x_parent;
  y->left = x;

  *node_rfp = y;
}

//     x          y
//    / \        / \
//   y   C  =>  A   x
//  / \            / \
// A   B          B   C
static void _node_rotate_right(_Node** node_rfp) {
  _Node* x = *node_rfp;

  bool x_red = x->red;
  _Node* x_parent = x->parent;
  _Node* y = x->left;

  bool y_red = y->red;
  _Node* b = y->right;

  if (b != NULL) {
    b->parent = x;
  }

  x->red = y_red;
  x->parent = y;
  x->left = b;

  y->red = x_red;
  y->parent = x_parent;
  y->right = x;

  *node_rfp = y;
}

const Layout map_layout = LAYOUT(_Map);

void map_init(
  void* map,
  Layout key_layout,
  Layout value_layout,
  Comparator comparator
) {
  map_init_with(map, key_layout, value_layout, comparator, heap_allocator);
}

void map_init_with(
  void* _map,
  Layout key_layout,
  Layout value_layout,
  Comparator comparator,
  Allocator allocator
) {
  _Map* map = _map;

  map->root = NULL;
  map->count = 0;
  map->key_layout = key_layout;
  map->value_layout = value_layout;
  map->comparator = comparator;
  map->allocator = allocator;

  Layout_builder builder = layout_init(0);
  layout_add(&builder, (Layout){.size = offsetof(_Node, data), .alignment = alignof(_Node)});
  map->node_layout.key_offset = layout_add(&builder, key_layout);
  map->node_layout.value_offset = layout_add(&builder, value_layout);

  Layout layout = layout_build(&builder);
  map->node_layout.size = layout.size;
  map->node_layout.alignment = layout.alignment;
}

size_t map_count(const void* _map) {
  const _Map* map = _map;
  return map->count;
}

void* map_get(const void* _map, const void* key) {
  const _Map* map = _map;

  _Node* node = map->root;

  while (node != NULL) {
    int order = comparator_compare(map->comparator, key, _node_key(node, &map->node_layout));

    if (order == 0)
      return _node_value(node, &map->node_layout);

    node = order < 0 ? node->left : node->right;
  }

  return NULL;
}

bool map_put(void* _map, const void* key, const void* value) {
  _Map* map = _map;

  // Top-down pass:

  _Node** node_rfp = &map->root;
  _Node* node = *node_rfp;
  _Node* parent = NULL;

  while (node != NULL) {
    int order = comparator_compare(map->comparator, key, _node_key(node, &map->node_layout));

    if (order == 0) {
      memmove(_node_value(node, &map->node_layout), value, map->value_layout.size);
      return true;
    }

    parent = node;
    node_rfp = order < 0 ? &node->left : &node->right;
    node = *node_rfp;
  }

  if ((node = allocator_allocate(map->allocator, map->node_layout.size)) == NULL)
    return false;

  node->red = true;
  node->parent = parent;
  node->left = NULL;
  node->right = NULL;
  memmove(_node_key(node, &map->node_layout), key, map->key_layout.size);
  memmove(_node_value(node, &map->node_layout), value, map->value_layout.size);
  *node_rfp = node;
  map->count += 1;

  // Bottom-up pass:

  while (parent != NULL) {
    if (parent->red) {
      if (parent == parent->parent->left) {
        if (node == parent->right) {
          _Node** parent_rfp = _node_rfp(parent, &map->root);
          _node_rotate_left(parent_rfp);
          parent = *parent_rfp;
        }

        _node_rotate_right(_node_rfp(parent->parent, &map->root));
      } else {
        if (node == parent->left) {
          _Node** parent_rfp = _node_rfp(parent, &map->root);
          _node_rotate_right(parent_rfp);
          parent = *parent_rfp;
        }

        _node_rotate_left(_node_rfp(parent->parent, &map->root));
      }
    }

    if (_node_black(parent->left) || _node_black(parent->right))
      break;

    _node_color_flip(parent);

    node = parent;
    parent = node->parent;
  }

  map->root->red = false;

  return true;
}

bool map_remove(void* _map, const void* key) {
  _Map* map = _map;

  // Top-down pass:

  _Node** node_rfp = &map->root;
  _Node* node = *node_rfp;

  while (true) {
    if (node == NULL)
      return false;

    int order = comparator_compare(map->comparator, key, _node_key(node, &map->node_layout));

    if (order == 0)
      break;

    node_rfp = order < 0 ? &node->left : &node->right;
    node = *node_rfp;
  }

  if (node->left != NULL && node->right != NULL) {
    _Node** pred_rfp = &node->left;
    _Node* pred = *pred_rfp;

    while (pred->right != NULL) {
      pred_rfp = &pred->right;
      pred = *pred_rfp;
    }

    memmove(
      _node_key(node, &map->node_layout),
      _node_key(pred, &map->node_layout),
      map->key_layout.size
    );

    memmove(
      _node_value(node, &map->node_layout),
      _node_value(pred, &map->node_layout),
      map->value_layout.size
    );

    node_rfp = pred_rfp;
    node = *node_rfp;
  }

  bool red = node->red;
  _Node* parent = node->parent;
  _Node* left = node->left;
  _Node* right = node->right;
  allocator_free(map->allocator, node);

  if (left != NULL) {
    left->red = red;
    left->parent = parent;
    *node_rfp = left;
    map->count -= 1;
    return true;
  }

  if (right != NULL) {
    right->red = red;
    right->parent = parent;
    *node_rfp = right;
    map->count -= 1;
    return true;
  }

  node = *node_rfp = NULL;
  map->count -= 1;

  // Bottom-up pass:

  if (red)
    return true;

  while (parent != NULL) {
    _Node** sibling_rfp = node == parent->left ? &parent->right : &parent->left;
    _Node* sibling = *sibling_rfp;

    if (_node_red(sibling)) {
      if (node == parent->left) {
        _node_rotate_left(_node_rfp(parent, &map->root));
      } else {
        _node_rotate_right(_node_rfp(parent, &map->root));
      }

      sibling = *sibling_rfp;
    }

    if (sibling != NULL) {
      sibling->red = true;

      if (_node_red(sibling->left) || _node_red(sibling->right)) {
        _Node** parent_rfp = _node_rfp(parent, &map->root);

        if (sibling == parent->right) {
          if (_node_black(sibling->right)) {
            _node_rotate_right(sibling_rfp);
          }

          _node_rotate_left(parent_rfp);
        } else {
          if (_node_black(sibling->left)) {
            _node_rotate_left(sibling_rfp);
          }

          _node_rotate_right(parent_rfp);
        }

        parent = *parent_rfp;
        parent->left->red = false;
        parent->right->red = false;
        return true;
      }
    }

    node = parent;

    if (node->red) {
      node->red = false;
      return true;
    }

    parent = node->parent;
  }

  return true;
}

static size_t _node_count(_Node* node) {
  return node == NULL ? 0 : 1 + _node_count(node->left) + _node_count(node->right);
}

static size_t _node_check(_Node* node) {
  if (node == NULL)
    return 1;

  assert(_node_black(node->parent) || !node->red);
  assert(node->left == NULL || node->left->parent == node);
  assert(node->right == NULL || node->right->parent == node);

  size_t left_black_height = _node_check(node->left);
  size_t right_black_height = _node_check(node->right);
  assert(left_black_height == right_black_height);

  return node->red ? left_black_height : 1 + left_black_height;
}

void map_check(const void* _map) {
  const _Map* map = _map;
  assert(map->count == _node_count(map->root));
  _node_check(map->root);
}
