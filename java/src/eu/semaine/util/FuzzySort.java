/**
 * Copyright (C) 2010 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.util;

import java.util.Arrays;
import java.util.Comparator;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;

import Jama.Matrix;

/**
 * This class implements an approximate sort operation on data for which only the following information is known:
 * <ul>
 * <li>direct order relations between pairs of items, as in "a wants to be a direct successor of b";</li>
 * <li>absolute positions at the ends of the chain, as in "a is initial" or "b is final"</li>.
 * </ul>
 * 
 * The class computes a mean square error approximation of the solution.
 * 
 * @author marc
 *
 */
public class FuzzySort {
	
	
	/**
	 * For a set of relations between fuzzy sortables, attempt to come up with a reasonable ordering despite circular orderings etc.
	 * A heuristic will be used to determine the threshold below which two items will be considered to be "equal" with respect to sorting.
	 * @param relations a non-null, non-empty set of relations
	 * @param enforceInitialFinal whether to interpret the initial/final requests by FuzzySortable as a soft or a hard constraint.
	 * If set to true, we guarantee that the first set in the returned list contains all initial items, and the last set in the list contains all final items. 
	 * @return a non-null, non-empty set of non-null, non-empty sets of sortables.
	 * @throws NullPointerException, IllegalArgumentException if relations is null or empty
	 */
	public static List<Set<FuzzySortable>> sort(Set<FuzzySortableRelation> relations, boolean enforceInitialFinal) {
		return sort(relations, enforceInitialFinal, Double.NaN);
	}

	/**
	 * For a set of relations between fuzzy sortables, attempt to come up with a reasonable ordering despite circular orderings etc.
	 * @param relations a non-null, non-empty set of relations
	 * @param enforceInitialFinal whether to interpret the initial/final requests by FuzzySortable as a soft or a hard constraint.
	 * If set to true, we guarantee that the first set in the returned list contains all initial items, and the last set in the list contains all final items.
	 * @param threshold the threshold below which two sortables are considered equal, given that each relation represents a requested distance of 1.
	 * This must be a non-negative value, or Float.NaN. If a value of 0 is given, a very small threshold is used; if NaN is given, a heuristic threshold is computed. 
	 * @return a non-null, non-empty set of non-null, non-empty sets of sortables.
	 * @throws NullPointerException, IllegalArgumentException if relations is null or empty, or if threshold is invalid
	 */
	public static List<Set<FuzzySortable>> sort(Set<FuzzySortableRelation> relations, boolean enforceInitialFinal, double threshold) {
		if (relations == null) {
			throw new NullPointerException("null argument");
		}
		if (relations.isEmpty()) {
			throw new IllegalArgumentException("empty argument");
		}
		if (threshold < 0) {
			throw new IllegalArgumentException("negative threshold");
		}
		// Identify the items to sort
		Set<FuzzySortable> itemSet = new HashSet<FuzzySortable>();
		Set<FuzzySortable> initialSet = enforceInitialFinal ? new HashSet<FuzzySortable>() : null;
		Set<FuzzySortable> finalSet = enforceInitialFinal ? new HashSet<FuzzySortable>() : null;
		for (FuzzySortableRelation r : relations) {
			FuzzySortable first = r.getFirst();
			if (enforceInitialFinal && first.isInitial()) {
				// keep item out of the itemset, its position is not to be computed but fixed
				initialSet.add(first);
			} else if (enforceInitialFinal && first.isFinal()) {
				// keep item out of the itemset, its position is not to be computed but fixed
				finalSet.add(first);
			} else {
				itemSet.add(first);
			}
			FuzzySortable second = r.getSecond();
			if (enforceInitialFinal && second.isInitial()) {
				// keep item out of the itemset, its position is not to be computed but fixed
				initialSet.add(second);
			} else if (enforceInitialFinal && second.isFinal()) {
				// keep item out of the itemset, its position is not to be computed but fixed
				finalSet.add(second);
			} else {
				itemSet.add(second);
			}
		}
//		System.out.println("Found "+itemSet.size()+" items");
		// Build up a numbering scheme for the matrix
		int numItems = itemSet.size();
		final FuzzySortable[] index2items = new FuzzySortable[numItems];
		final Map<FuzzySortable, Integer> items2index = new HashMap<FuzzySortable, Integer>();
		int i = 0;
		for (FuzzySortable item : itemSet) {
			index2items[i] = item;
			items2index.put(item, i);
			i++;
		}
		
		// Positions of initial / final items:
		// (these are fixed if enforceInitialFinal is true, otherwise they are target positions)
		double posInitial = 0;
		double posFinal = numItems + (enforceInitialFinal ? initialSet.size() + finalSet.size() : 0);
		double posOthers = posFinal / 2.;
		
		final double equalityThreshold;
		if (threshold == 0) {
			equalityThreshold = 1.e-9;
		} else if (Double.isNaN(threshold)) {
			equalityThreshold = posFinal / 50.;
		} else {
			equalityThreshold = threshold;
		}
		
		// Fill the matrix of constraints
		int numAbsolute = numItems;
		int numRelations = relations.size();
		// The variables in this matrix (=the columns) are the positions of the items
		double[][] A = new double[numAbsolute+numRelations][numItems];
		double[][] b = new double[numAbsolute+numRelations][1];
		// Absolute position constraints: These take initial/final into account and make sure the matrix is never singular.
		// - initial items are at position 0;
		// - final items are at position numItems;
		// - all others are at position numItems/2.
		for(int j=0; j<numAbsolute; j++) {
			FuzzySortable item = index2items[j]; 
			double targetPos;
			if (item.isInitial()) {
				targetPos = posInitial;
			} else if (item.isFinal()) {
				targetPos = posFinal;
			} else {
				targetPos = posOthers;
			}
			A[j][j] = 1;
			b[j][0] = targetPos;
		}
		Iterator<FuzzySortableRelation> relationIterator = relations.iterator();
		for (int j=numAbsolute; j<numAbsolute+numRelations; j++) {
			// for each relation, we state that the position of the two items in the relation differs by one:
			FuzzySortableRelation r = relationIterator.next();
			FuzzySortable first = r.getFirst();
			FuzzySortable second = r.getSecond();
			if (enforceInitialFinal && (first.isInitial() || first.isFinal() || second.isInitial() || second.isFinal())) { 
				// one of both is not part of the optimisation:
				if ((first.isInitial() || first.isFinal()) && (second.isInitial() || second.isFinal())) {
					continue; // no constraints here
				} else if (first.isInitial() || first.isFinal()) {
					// constaint on second to follow posInitial / posFinal (?)
					int kSecond = items2index.get(second);
					A[j][kSecond] = 1;
					b[j][0] = (first.isInitial() ? posInitial : posFinal) + 1;
				} else if (second.isInitial() || second.isFinal) {
					// constraint on first to precede posInitial (?) / posFinal
					int kFirst = items2index.get(first);
					A[j][kFirst] = 1;
					b[j][0] = (second.isInitial() ? posInitial : posFinal) - 1;
				}
			} else {
				// Both sortables are part of the optimisation:
				int kFirst = items2index.get(first);
				int kSecond = items2index.get(second);
				// position of second - position of first = 1
				A[j][kFirst] = -1;
				A[j][kSecond] = 1;
				b[j][0] = 1;
			}
		}
		
//		new Matrix(A).print(3, 0);
//		new Matrix(b).print(3, 0);
		
		// Compute the least squares solution
        Matrix x = new Matrix(A).solve(new Matrix(b));
        final double[] rawPositions = new double[numItems];
        for (int k=0; k<numItems; k++) {
        	rawPositions[k] = x.get(k, 0);
        }
		// Condense solution into an order relation
		FuzzySortable[] sortedItems = index2items.clone();
		Arrays.sort(sortedItems, new Comparator<FuzzySortable>() {
			@Override
			public int compare(FuzzySortable o1, FuzzySortable o2) {
				return FuzzySort.compare(o1, o2, rawPositions, items2index, equalityThreshold);
			}
		});
		LinkedList<Set<FuzzySortable>> result = new LinkedList<Set<FuzzySortable>>();
		Set<FuzzySortable> currentSet = null;
		FuzzySortable prevSortable = null;
		for (int k=0; k<numItems; k++) {
			FuzzySortable currentSortable = sortedItems[k];
        	System.out.println("Position of item "+currentSortable.toString()+" is "+rawPositions[items2index.get(currentSortable)]);
			if (currentSet == null) {
				currentSet = new HashSet<FuzzySortable>();
				result.add(currentSet);
			} else {
				assert prevSortable != null;
				int cmp = compare(prevSortable, currentSortable, rawPositions, items2index, equalityThreshold);
				assert cmp <= 0;
				if (cmp < 0) { // not equal
					currentSet = new HashSet<FuzzySortable>();
					result.add(currentSet);
				} // else, add currentSortable to same set
			}
			currentSet.add(currentSortable);
			prevSortable = currentSortable;
		}
		// And finally, if requested, move all initial/final ones to the two ends
		if (enforceInitialFinal) {
			if (!initialSet.isEmpty()) {
				result.addFirst(initialSet);
			}
			if (!finalSet.isEmpty()) {
				result.addLast(finalSet);
			}
		}
		
//		System.out.println("Sorted items into "+result.size()+" sets");
//		System.out.println("Sorted result: ");
//		System.out.println(toString(result));
		return result;
	}
	
	/**
	 * Compare the positions of o1 and o2 given the raw positions.
	 * @param o1 one of the two items to compare
	 * @param o2 the other item to compare
	 * @param rawPositions the positions of items on a sorted axis
	 * @param items2index the mapper from objects to index numbers in rawPositions
	 * @return an integer smaller than, equal to, or greater than 0 if o1 is less than, equal to, or greather than o2.
	 * Note that o1 and o2 are compared equal if their rawPositions are less than EPSILON apart.
	 */
	private static int compare(FuzzySortable o1, FuzzySortable o2,
			final double[] rawPositions, final Map<FuzzySortable, Integer> items2index, double threshold) {
		int index1 = items2index.get(o1);
		int index2 = items2index.get(o2);
		if (Math.abs(rawPositions[index1]-rawPositions[index2]) < threshold) {
			return 0;
		}
		return Double.compare(rawPositions[index1], rawPositions[index2]);
	}	
	
	
	/**
	 * Convert the sorted results into a human-readable string. Each line contains one set of sortables;
	 * items in each line are separated by a white space character.
	 * @param sortedResults non-null, non-empty list of non-null, non-empty sets of fuzzy sortables
	 * @return a non-null string representing sortedResult
	 * @throws IllegalArgumentException, NullPointerException when the parameter is not conformant
	 */
	public static String toString(List<Set<FuzzySortable>> sortedResults) {
		if (sortedResults == null) {
			throw new NullPointerException("null argument");
		}
		if (sortedResults.size() == 0) {
			throw new IllegalArgumentException("empty argument");
		}
		StringBuilder sb = new StringBuilder();
		for (Set<FuzzySortable> set : sortedResults) {
			if (set == null) {
				throw new NullPointerException("null set in argument");
			}
			if (set.size() == 0) {
				throw new IllegalArgumentException("empty set in argument");
			}
			for (FuzzySortable s : set) {
				sb.append(s.toString()).append(" ");
			}
			sb.append("\n");
		}
		return sb.toString();
	}
	
	
	
	
	
	///////////////////////// classes ////////////////////////

	public static class FuzzySortable {
		private final boolean isInitial;
		private final boolean isFinal;
		private final Object payload;
		
		/**
		 * Create a fuzzy sortable which is neither initial nor final.
		 * @param payload an object associated with this sortable. must not be null
		 * @throws NullPointerException if payload is null
		 */
		public FuzzySortable(Object payload) {
			this(payload, false, false);
		}
		
		/**
		 * Create a fuzzy sortable and set its initial or final state. A sortable cannot be both initial and final.
		 * @param payload an object associated with this sortable. must not be null
		 * @param isInitial whether the sortable wants to be in initial position
		 * @param isFinal whether the sortable wants to be in final position
		 * @throws IllegalArgumentException if both isInitial and isFinal are true.
		 * @throws NullPointerException if payload is null
		 */
		public FuzzySortable(Object payload, boolean isInitial, boolean isFinal) 
		throws IllegalArgumentException {
			if (isInitial && isFinal) {
				throw new IllegalArgumentException("Cannot create a sortable that is both initial and final");
			}
			if (payload == null) {
				throw new NullPointerException("payload must not be null");
			}
			this.payload = payload;
			this.isInitial = isInitial;
			this.isFinal = isFinal;
		}
		
		/**
		 * Indicate whether this sortable wants to be in initial position.
		 * @return
		 */
		public boolean isInitial() {
			return isInitial;
		}

		/**
		 * Indicate whether this sortable wants to be in final position.
		 * @return
		 */
		public boolean isFinal() {
			return isFinal;
		}
		
		public Object getPayload() {
			return payload;
		}
		
		@Override
		public String toString() {
			return payload.toString();
		}
		
		@Override
		public int hashCode() {
			return payload.hashCode();
		}
		
		@Override
		public boolean equals(Object obj) {
			if (!(obj instanceof FuzzySortable)) {
				return false;
			}
			FuzzySortable other = (FuzzySortable) obj;
			return other.getPayload().equals(payload);
		}
	}
	
	public static class FuzzySortableRelation {
		private FuzzySortable first;
		private FuzzySortable second;
		private boolean haveHashCode = false;
		private int hashCode = 0;
		
		/**
		 * Represent a desirect direct order relation between two sortables.
		 * @param first a fuzzy sortable (must not be null)
		 * @param second another fuzzy sortable, which wants to immediately follow first (must not be null)
		 * @throws NullPointerException if one is null
		 * @throws IllegalArgumentException if both are equal
		 */
		public FuzzySortableRelation(FuzzySortable first, FuzzySortable second) throws NullPointerException {
			if (first == null || second == null) {
				throw new NullPointerException("received null argument");
			}
			if (first.equals(second)) {
				throw new IllegalArgumentException("the two sortables in the relation must not be equal");
			}
			this.first = first;
			this.second = second;
		}
		
		/**
		 * get the sortable in this relation that wants to immediately precede the other one
		 * @return a fuzzy sortable, guaranteed not to be null
		 */
		public FuzzySortable getFirst() {
			assert first != null;
			return first;
		}
		
		/**
		 * get the sortable in this relation that wants to immediately follow the other one
		 * @return a fuzzy sortable, guaranteed not to be null
		 */
		public FuzzySortable getSecond() {
			assert second != null;
			return second;
		}
		
		@Override
		public boolean equals(Object o) {
			if (!(o instanceof FuzzySortableRelation)) {
				return false;
			}
			FuzzySortableRelation other = (FuzzySortableRelation) o;
			return other.first.equals(first) && other.second.equals(second);
		}
		
		@Override
		public int hashCode() {
			if (!haveHashCode) {
				hashCode = first.hashCode() << 2 + second.hashCode();
				haveHashCode = true;
			}
			return hashCode;
		}
	}
}
