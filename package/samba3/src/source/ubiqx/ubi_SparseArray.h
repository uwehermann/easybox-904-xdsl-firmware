#ifndef UBI_SPARSEARRAY_H
#define UBI_SPARSEARRAY_H
/* ========================================================================== **
 *                             ubi_SparseArray.h
 *
 *  Copyright (C) 1999 by Christopher R. Hertel
 *
 *  Email: crh@ubiqx.mn.org
 * -------------------------------------------------------------------------- **
 *  This module implements a binary tree based sparse array.
 * -------------------------------------------------------------------------- **
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * -------------------------------------------------------------------------- **
 * This is a simple implementation of a sparse array based on binary trees.
 * The default is to use ubi_SplayTree, but this can be changed by altering
 * the #include below.
 *
 * This implementation adds a pointer to a child tree to the node structures.
 * This allows a child tree to be to be connected at any node in a given
 * binary tree.  In addition, the tree root structure contains pointers to
 * link back to the parent tree and parent node.
 *
 * Note that when we talk about a child tree we are talking about an entire
 * tree, not a subset of the current tree.  This adds a new dimention at
 * *each* node.  For that reason, we sometimes refer to the child trees as
 * vectors.
 *
 * Using these new pointers (from node to child tree and from child tree root
 * back to parent node and parent tree), it is easy to traverse the sparse
 * array.  Traversal within a vector is handled by the binary tree functions.
 *
 * -------------------------------------------------------------------------- **
 *
 * $Log$
 *
 * ========================================================================== **
 */

#include "ubi_SplayTree.h"


/* -------------------------------------------------------------------------- **
 * Typedefs...
 *
 *  ubi_arrRoot     - The root of a single vector within the sparse array.
 *  ubi_arrRootPtr  - Pointer to ubi_arrRoot.
 *  ubi_arrNode     - A node which can be stored within a vector.
 *  ubi_arrNodePtr  - Pointer to a ubi_arrNode.
 */

struct ubi_arrNODE;

typedef struct ubi_arrROOT
  {
  ubi_trRoot          root;         /* Tree Header for the sub-array. */
  struct ubi_arrROOT *parentArray;  /* Pointer to the parent array.   */
  struct ubi_arrNODE *parentNode;   /* Pointer to the parent node.    */
  } ubi_arrRoot;

typedef ubi_arrRoot *ubi_arrRootPtr;


typedef struct ubi_arrNODE
  {
  ubi_trNode     node;        /* Standard tree node structure.  */
  ubi_arrRootPtr subArray;    /* Link to potential child array. */
  } ubi_arrNode;

typedef ubi_arrNode *ubi_arrNodePtr;


/* -------------------------------------------------------------------------- **
 * Function Prototypes...
 */

ubi_arrRootPtr ubi_arrInitRoot( ubi_arrRootPtr RootPtr,
                                ubi_trCompFunc CompFunc,
                                char           Flags );
  /* ------------------------------------------------------------------------ **
   * Initialize a vector root.
   *
   *  Input:  RootPtr   - Pointer to the ubi_arrRoot to be initialized.
   *          CompFunc  - Comparison function to be used to sort the entries
   *                      in this vector.
   *          Flags     - Available flags are listed in ubi_BinTree.h.
   *
   *  Output: A pointer to the initialized root structure (that is, the same
   *          as RootPtr).
   *
   *  Notes:  This is the same as initializing a binary tree root, except
   *          that there are additional pointers to the parent tree and node.
   *          These are initialized to NULL.  Use ubi_arrAddSubArray() to
   *          add this tree to a node in another tree.
   *
   * ------------------------------------------------------------------------ **
   */

ubi_arrNodePtr ubi_arrInitNode( ubi_arrNodePtr NodePtr );
  /* ------------------------------------------------------------------------ **
   * Initialize a sparse array node structure.
   *
   *  Input:  NodePtr - Pointer to the ubi_arrNode to be initialized.
   *
   *  Output: Pointer to the initialized node (that is, the same as NodePtr).
   *
   * ------------------------------------------------------------------------ **
   */

ubi_arrRootPtr ubi_arrDown( ubi_arrNodePtr NodePtr );
  /* ------------------------------------------------------------------------ **
   * Returns a pointer to the vector associated with <NodePtr>, or NULL if
   * there is no associated vector.
   *
   *  Input:  NodePtr - pointer to the link node.
   *
   *  Output: A pointer to the subarray attached at *NodePtr, or NULL if
   *          *NodePtr does not point to a subarray.
   *
   *  Notes:  This operation can be thought of as a move to a new dimention.
   *          Using an analogy from C, if you are looking at X[6,3,4], this
   *          function would return a pointer to X[6,3,4,0].  Sort of.
   *
   * ------------------------------------------------------------------------ **
   */

ubi_arrRootPtr ubi_arrUp( ubi_arrRootPtr RootPtr, ubi_arrNodePtr *parentp );
  /* ------------------------------------------------------------------------ **
   * Given a pointer to a vector root, return a pointer to the vector that
   * contains the parent node (if there is one).
   *
   *  Input:  RootPtr - pointer to a vector root.
   *          parentp - pointer to a ubi_arrNodePtr.  If NULL, this value
   *                    will be ignored.  If non-NULL, the indicated pointer
   *                    will be set to point to the node which is the
   *                    parent of *RootPtr.
   *
   *  Output: A pointer to the root of the parent vector.  If RootPtr points
   *          to a top-level vector, then the return value will be NULL (and,
   *          if parentp != NULL, then *parentp will also be set to NULL).
   *
   * ------------------------------------------------------------------------ **
   */

ubi_arrRootPtr ubi_arrTop( ubi_arrRootPtr RootPtr );
  /* ------------------------------------------------------------------------ **
   * Return a pointer to the top-most tree in the sparse array.
   *
   *  Input:  RootPtr - Pointer to the root of a tree (vector) within the
   *                    sparse array.
   *
   *  Output: A pointer to the top-most root in the sparse array.  If RootPtr
   *          is NULL, then the array is the 'empty' array and NULL will be
   *          returned.
   *
   * ------------------------------------------------------------------------ **
   */

ubi_arrRootPtr ubi_arrAddSubArray( ubi_arrRootPtr NewRootPtr,
                                   ubi_arrRootPtr ParentRootPtr,
                                   ubi_arrNodePtr ParentNodePtr );
  /* ------------------------------------------------------------------------ **
   * Add a vector header (tree root) at a given node.
   *
   *  Input:  NewRootPtr    - Pointer to the root of the vector (tree) that
   *                          is to be added as a child array at the node
   *                          indicated by ParentNodePtr.
   *          ParentRootPtr - Pointer to the root of the vector (tree) that
   *                          contains ParentNodePtr.
   *          ParentNodePtr - The node at which the new vector will be
   *                          attached.
   *
   *  Output: Pointer to the root of the newly added vector (that is, the
   *          same as NewRootPtr).
   *
   * ------------------------------------------------------------------------ **
   */

ubi_arrRootPtr ubi_arrRemSubArray( ubi_arrNodePtr NodePtr );
  /* ------------------------------------------------------------------------ **
   * Remove a vector from a node.
   *
   *  Input:  NodePtr - Pointer to the node at which the vector to be removed
   *                    is connected.
   *
   *  Output: Pointer to the root of the vector that was disconnected.  If
   *          no vector was found, the function will return NULL.
   *
   * ------------------------------------------------------------------------ **
   */

/* ================================ The End ================================= */
#endif /* UBI_SPARSEARRAY_H */
