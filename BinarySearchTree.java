public class BinarySearchTree<T extends Comparable<? super T>> {
	private BinaryNode<T> root; // root node for the object

	/**
	 * Constructor for BinarySearchTree
	 * 
	 * @return a Binary Search Tree (BST)
	 */
	public BinarySearchTree() {
		root = null;
	}

	/**
	 * Adds an item to the BST
	 * 
	 * @return void
	 */
	public void insert(T item) {
		insert(root, item);
	}

	private void insert(BinaryNode<T> r, T item) { //Part of the above method
		if (r == null) {
			root = new BinaryNode(item);
		} else {
			if ((r.data).compareTo(item) > 0) {
				if (r.leftChild == null) {
					r.leftChild = new BinaryNode(item);
				} else
					insert(r.leftChild, item);
			} else {
				if (r.rightChild == null) {
					r.rightChild = new BinaryNode(item);
				} else
					insert(r.rightChild, item);
			}
		}
	}

	/**
	 * Displays inorder traversal of BST
	 * 
	 * @return void
	 */
	public void displayIn() {
		System.out.println("Displaying in order traversal");
		displayIn(root);
		System.out.println();
	}

	private void displayIn(BinaryNode<T> r) { //Part of the above method
		if (r != null) {
			displayIn(r.leftChild);
			System.out.print(r.data + "   ");
			displayIn(r.rightChild);
		}
	}

	/**
	 * Recursively counts nodes in BST
	 * 
	 * @return int number of nodes
	 */
	public int countNodes() {
		return countNodes(root);
	}

	private int countNodes(BinaryNode<T> r) { //part of the above method
		if (r == null)
			return 0;
		return 1 + countNodes(r.rightChild) + countNodes(r.leftChild);
	}
	
	/**
	 * Recursively counts levels in BST
	 * 
	 * @return int number of levels
	 */
	public int countLevels() {
		int count = 0;
		return countLevels(root, count);
	}
	
	private int countLevels(BinaryNode<T> r, int count) { //part of the above method
		if(r==null)
			return count;
		count++;
		int rightSide = countLevels(r.rightChild, count);
		int leftSide = countLevels(r.leftChild, count);
		return java.lang.Math.max(rightSide, leftSide);
	}

	/**
	 * Displays preorder traversal of BST
	 * 
	 * @return void
	 */
	public void displayPre() {
		System.out.println("Displaying Preorder Traversal");
		displayPre(root);
		System.out.println();
	}

	private void displayPre(BinaryNode<T> r) {
		if (r != null) {
			System.out.print(r.data + "   ");
			displayIn(r.leftChild);
			displayIn(r.rightChild);
		}
	}

	/**
	 * Removes a specific node
	 *
	 * @return node to remove if node not found
	 */
	private BinaryNode<T> getNodeToRemove(BinaryNode<T> currentNode) { // Find node with largest entry in left subtree
																		// by
		// moving as far right in the subtree as possible
		BinaryNode<T> leftSubtreeRoot = currentNode.getLeftChild();
		BinaryNode<T> rightChild = leftSubtreeRoot;
		BinaryNode<T> priorNode = currentNode;
		while (rightChild.hasRightChild()) {
			priorNode = rightChild;
			rightChild = rightChild.getRightChild();
		} // end while
			// rightChild contains the inorder predecessor and is the node to
			// remove; priorNode is its parent
		return rightChild;
	} // end getNodeToRemov

	// testing above method
	public void testNTR() {
		System.out.println("getNodeToRemove(root).getData(): " + getNodeToRemove(root).getData());
	} // works as of 5/15

	/**
	 * Finding a node that matched a given value
	 *
	 * @return null if node not found
	 */
	private BinaryNode<T> findNode(T entry) {
		BinaryNode<T> current = root;
		boolean found = false;
		while (!found) {
			if (current == null)
				return null; // if value is not found
			T currentEntry = current.getData();
			int comparison = entry.compareTo(currentEntry);

			if (comparison == 0) {
				found = true;
			} else if (comparison < 0) {
				current = current.leftChild;
			} else {
				assert comparison > 0;
				current = current.rightChild;
			}
		}
		return current;
	} // end findNod

	// tests above fcn
	public void testFN(T entry) {
		BinaryNode<T> result = findNode(entry);
		System.out.println("result.getLeftChild().getData(): " + result.getLeftChild().getData()
				+ "\tresult.getRightChild().getData(): " + result.getRightChild().getData());
	} // works as of 5/15

	class BinaryNode<T> {
		private T data;
		private BinaryNode<T> leftChild; // Reference to left child
		private BinaryNode<T> rightChild; // Reference to right child

		public BinaryNode() {
			this(null); // Call next constructor
		} // end default constructor

		public BinaryNode(T dataPortion) {
			this(dataPortion, null, null); // Call next constructor
		} // end constructor

		public BinaryNode(T dataPortion, BinaryNode<T> newLeftChild, BinaryNode<T> newRightChild) {
			data = dataPortion;
			leftChild = newLeftChild;
			rightChild = newRightChild;
		} // end constructor

		/**
		 * Retrieves the data portion of this node.
		 * 
		 * @return The object in the data portion of the node.
		 */
		public T getData() {
			return data;
		} // end getData

		/**
		 * Sets the data portion of this node.
		 * 
		 * @param newData The data object.
		 */
		public void setData(T newData) {
			data = newData;
		} // end setData

		/**
		 * Retrieves the left child of this node.
		 * 
		 * @return The node's left child.
		 */
		public BinaryNode<T> getLeftChild() {
			return leftChild;
		} // end getLeftChild

		/**
		 * Sets this node's left child to a given node.
		 * 
		 * @param newLeftChild A node that will be the left child.
		 */
		public void setLeftChild(BinaryNode<T> newLeftChild) {
			leftChild = newLeftChild;
		} // end setLeftChild

		/**
		 * Detects whether this node has a left child.
		 * 
		 * @return True if the node has a left child.
		 */
		public boolean hasLeftChild() {
			return leftChild != null;
		} // end hasLeftChild

		/**
		 * Retrieves the right child of this node.
		 * 
		 * @return The node's right child.
		 */
		public BinaryNode<T> getRightChild() {
			return rightChild;
		} // end getRightChild

		/**
		 * Sets this node's right child to a given node.
		 * 
		 * @param newRightChild A node that will be the right child.
		 */
		public void setRightChild(BinaryNode<T> newRightChild) {
			rightChild = newRightChild;
		} // end setRightChild

		/**
		 * Detects whether this node has a right child.
		 * 
		 * @return True if the node has a right child.
		 */
		public boolean hasRightChild() {
			return rightChild != null;
		} // end hasRightChild

		/**
		 * Detects whether this node is a leaf.
		 * 
		 * @return True if the node is a leaf.
		 */
		public boolean isLeaf() {
			return (leftChild == null) && (rightChild == null);
		} // end isLeaf
	} // end BinaryNode
}
