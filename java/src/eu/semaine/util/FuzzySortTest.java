/**
 * Copyright (C) 2010 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.util;

import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Set;

import org.junit.Test;

import eu.semaine.util.FuzzySort.FuzzySortable;
import eu.semaine.util.FuzzySort.FuzzySortableRelation;
import static org.junit.Assert.*;

/**
 * @author marc
 *
 */
public class FuzzySortTest {
	// Utility methods

	private final FuzzySortable a = new FuzzySortable("a");
	private final FuzzySortable b = new FuzzySortable("b");
	private final FuzzySortable c = new FuzzySortable("c");
	private final FuzzySortable d = new FuzzySortable("d");
	private final FuzzySortable e = new FuzzySortable("e");
	private final FuzzySortable initial1 = new FuzzySortable("i1", true, false);
	private final FuzzySortable initial2 = new FuzzySortable("i2", true, false);
	private final FuzzySortable final1 = new FuzzySortable("f1", false, true);
	private final FuzzySortable final2 = new FuzzySortable("f2", false, true);
	
	private final FuzzySortable[] linearSolution = new FuzzySortable[] {
			a, b, c, d, e
	};
	
	private final FuzzySortable[] linearInitFinalSolution = new FuzzySortable[] {
			initial1, a, b, c, d, e, final2
	};
	
	
	private Set<FuzzySortableRelation> getSimpleConstraintsForLinearSolution() {
		// Intended order: a b c d e
		Set<FuzzySortableRelation> r = new HashSet<FuzzySortableRelation>();
		r.add(new FuzzySortableRelation(a, b));
		r.add(new FuzzySortableRelation(b, c));
		r.add(new FuzzySortableRelation(c, d));
		r.add(new FuzzySortableRelation(d, e));
		return r;
	}
	
	
	private Set<FuzzySortableRelation> getInitFinalConstrainsForLinearSolution() {
		// Intended order: initial1 a b c d e final2
		Set<FuzzySortableRelation> r = new HashSet<FuzzySortableRelation>();
		r.add(new FuzzySortableRelation(final2, initial1)); // !!
		r.add(new FuzzySortableRelation(a, b));
		r.add(new FuzzySortableRelation(b, c));
		r.add(new FuzzySortableRelation(c, d));
		r.add(new FuzzySortableRelation(d, e));
		return r;
	}
	
	
	private Set<FuzzySortableRelation> getConflictingConstraintsForLinearSolution() {
		// Intended order: a b c d e
		Set<FuzzySortableRelation> r = new HashSet<FuzzySortableRelation>();
		r.add(new FuzzySortableRelation(a, b));
		r.add(new FuzzySortableRelation(b, c));
		r.add(new FuzzySortableRelation(c, d));
		r.add(new FuzzySortableRelation(b, e)); // !!
		r.add(new FuzzySortableRelation(d, e));
		return r;
	}
	

	private final FuzzySortable[][] parallelSolution1 = new FuzzySortable[][] {
			new FuzzySortable[] { initial1 },
			new FuzzySortable[] { a },
			new FuzzySortable[] { b, c },
			new FuzzySortable[] { d }
	};
	
	private Set<FuzzySortableRelation> getSimpleConstraintsForParallelSolution() {
		// Intended order: initial1 a b|c d
		Set<FuzzySortableRelation> r = new HashSet<FuzzySortableRelation>();
		r.add(new FuzzySortableRelation(initial1, a));
		r.add(new FuzzySortableRelation(a, b));
		r.add(new FuzzySortableRelation(a, c));
		r.add(new FuzzySortableRelation(b, d));
		r.add(new FuzzySortableRelation(c, d));
		return r;
	}
	
	private Set<FuzzySortableRelation> getConflictingConstraintsForParallelSolution() {
		// Intended order: initial1 a b|c d
		Set<FuzzySortableRelation> r = new HashSet<FuzzySortableRelation>();
		r.add(new FuzzySortableRelation(initial1, a));
		r.add(new FuzzySortableRelation(a, b));
		r.add(new FuzzySortableRelation(a, c));
		r.add(new FuzzySortableRelation(b, d));
		r.add(new FuzzySortableRelation(c, d));
		r.add(new FuzzySortableRelation(d, a));
		return r;
	}
	
	
	private void assertLinearSolutionCorrect(List<Set<FuzzySortable>> candidate, FuzzySortable[] solution) throws AssertionError {
		if (candidate.size() != solution.length) {
			throw new AssertionError("length does not match");
		}
		for (int i=0; i<solution.length; i++) {
			Set<FuzzySortable> oneItem = candidate.get(i);
			assertEquals(1, oneItem.size());
			assertEquals(solution[i], oneItem.iterator().next());
		}
	}
	
	private void assertParallelSolutionCorrect(List<Set<FuzzySortable>> candidate, FuzzySortable[][] solution) throws AssertionError {
		if (candidate.size() != solution.length) {
			throw new AssertionError("length does not match");
		}
		for (int i=0; i<solution.length; i++) {
			Set<FuzzySortable> oneCandidate = candidate.get(i);
			FuzzySortable[] oneSolution = solution[i];
			assertEquals(oneSolution.length, oneCandidate.size());
			Iterator<FuzzySortable> it = oneCandidate.iterator();
			for (int j=0; j<oneSolution.length; j++) {
				assertTrue(oneCandidate.contains(oneSolution[j]));
			}
		}
	}
	
	
	
	
	
	// Tests

	/**
	 * Create a sortable and verify that it looks as intended
	 */
	@Test
	public void canCreateSortable() {
		// setup
		FuzzySortable f = new FuzzySortable("");
		// verify: it is neither initial nor final
		assertFalse(f.isInitial());
		assertFalse(f.isFinal());
	}

	@Test
	public void canCreateSortable2() {
		// setup
		FuzzySortable f = new FuzzySortable("", false, false);
		// verify: it is neither initial nor final
		assertFalse(f.isInitial());
		assertFalse(f.isFinal());
	}

	@Test
	public void canCreateInitialSortable() {
		// setup
		FuzzySortable f = new FuzzySortable("", true, false);
		// verify
		assertTrue(f.isInitial());
	}

	@Test
	public void canCreateFinalSortable() {
		// setup
		FuzzySortable f = new FuzzySortable("", false, true);
		// verify
		assertTrue(f.isFinal());
	}

	@Test
	public void cannotCreateInitialFinalSortable() {
		try {
			new FuzzySortable("", true, true);
			fail("Should not allow creation of initial and final sortable");
		} catch (IllegalArgumentException e) {
			// OK, expected
		}
	}
	
	@Test
	public void payloadPreserved() {
		// setup
		Object payload = new Object();
		// exercise
		FuzzySortable f = new FuzzySortable(payload);
		// verify
		assertTrue(f.getPayload() == payload); // identical object
	}

	/**
	 * establish the order relationship between sortables
	 */
	@Test
	public void canRelateSortables() {
		// setup
		FuzzySortable first = new FuzzySortable("first");
		FuzzySortable second = new FuzzySortable("second");
		// exercixe
		FuzzySortableRelation r = new FuzzySortableRelation(first, second);
		// verify
		assertEquals(r.getFirst(), first);
		assertEquals(r.getSecond(), second);
	}

	@Test
	public void cannotRelateNullSortables1() {
		// setup
		FuzzySortable f = new FuzzySortable("");
		// exercise
		try {
			new FuzzySortableRelation(f, null);
			fail("should have thrown a null pointer exception");
		} catch (NullPointerException npe) {
			// OK, expected
		}
	}

	@Test
	public void cannotRelateNullSortables2() {
		// setup
		FuzzySortable f = new FuzzySortable("");
		// exercise
		try {
			new FuzzySortableRelation(null, f);
			fail("should have thrown a null pointer exception");
		} catch (NullPointerException npe) {
			// OK, expected
		}
	}
	
	@Test
	public void cannotRelateToEqual() {
		// setup
		FuzzySortable f = new FuzzySortable("");
		// exercise
		try {
			new FuzzySortableRelation(f, f);
			fail("should have thrown illegal argument exception");
		} catch (IllegalArgumentException e) {
			// OK, expected
		}
	}
	
	@Test
	public void relationsAreEquals() {
		// setup
		FuzzySortable first = new FuzzySortable("first");
		FuzzySortable second = new FuzzySortable("second");
		FuzzySortableRelation r1 = new FuzzySortableRelation(first, second);
		// exercise
		FuzzySortableRelation r2 = new FuzzySortableRelation(first, second);
		// verify
		assertEquals(r1, r2);
		assertTrue(r1.hashCode() == r2.hashCode());
		
	}

	@Test
	public void relationsAreNotEquals() {
		// setup
		FuzzySortable first = new FuzzySortable("first");
		FuzzySortable second = new FuzzySortable("second");
		FuzzySortableRelation r1 = new FuzzySortableRelation(first, second);
		// exercise
		FuzzySortableRelation r2 = new FuzzySortableRelation(second, first);
		// verify
		assertNotSame(r1, r2);
		assertTrue(r1.hashCode() != r2.hashCode());
		
	}

	@Test
	public void canSortLinear1() {
		// Setup
		Set<FuzzySortableRelation> constraints = getSimpleConstraintsForLinearSolution();
		// exercise
		List<Set<FuzzySortable>> solution = FuzzySort.sort(constraints);
		// verify result
		assertLinearSolutionCorrect(solution, linearSolution);
	}
	
	@Test
	public void canSortLinear2() {
		// Setup
		Set<FuzzySortableRelation> constraints = getInitFinalConstrainsForLinearSolution();
		// exercise
		List<Set<FuzzySortable>> solution = FuzzySort.sort(constraints);
		// verify result
		assertLinearSolutionCorrect(solution, linearInitFinalSolution);
	}
	
	@Test
	public void canSortLinear3() {
		// Setup
		Set<FuzzySortableRelation> constraints = getConflictingConstraintsForLinearSolution();
		// exercise
		List<Set<FuzzySortable>> solution = FuzzySort.sort(constraints);
		// verify result
		assertLinearSolutionCorrect(solution, linearSolution);
	}
	
	
	@Test
	public void canSortParallel1() {
		// setup
		Set<FuzzySortableRelation> constraints = getSimpleConstraintsForParallelSolution();
		// exercise
		List<Set<FuzzySortable>> solution = FuzzySort.sort(constraints);
		// verify result
		assertParallelSolutionCorrect(solution, parallelSolution1);
	}
	
	@Test
	public void canSortParallel2() {
		// setup
		Set<FuzzySortableRelation> constraints = getConflictingConstraintsForParallelSolution();
		// exercise
		List<Set<FuzzySortable>> solution = FuzzySort.sort(constraints);
		// verify result
		assertParallelSolutionCorrect(solution, parallelSolution1);
	}

}
