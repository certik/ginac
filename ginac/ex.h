/** @file ex.h
 *
 *  Interface to GiNaC's light-weight expression handles. */

/*
 *  GiNaC Copyright (C) 1999-2003 Johannes Gutenberg University Mainz, Germany
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __GINAC_EX_H__
#define __GINAC_EX_H__

#include <iosfwd>
#include <iterator>
#include <functional>

#include "basic.h"

namespace GiNaC {

/** Helper class to initialize the library.  There must be one static object
 *  of this class in every object file that makes use of our flyweights in
 *  order to guarantee proper initialization.  Hence we put it into this
 *  file which is included by every relevant file anyways.  This is modeled
 *  after section 27.4.2.1.6 of the C++ standard, where cout and friends are
 *  set up.
 *
 *  @see utils.cpp */
class library_init {
public:
	library_init();
	~library_init();
private:
	static int count;
};
/** For construction of flyweights, etc. */
static library_init library_initializer;

// Current versions of Cint don't link data declared extern within functions.
// FIXME: Fix Cint and later remove this from here.
#if defined(G__CINTVERSION)
extern const class numeric *_num0_p;
#endif


class symbol;
class lst;
class scalar_products;


/** Lightweight wrapper for GiNaC's symbolic objects.  Basically all it does is
 *  to hold a pointer to the other objects, manage the reference counting and
 *  provide methods for manipulation of these objects.  (Some people call such
 *  a thing a proxy class.) */
class ex
{
	friend class archive_node;
	friend bool are_ex_trivially_equal(const ex &, const ex &);
	template<class T> friend const T &ex_to(const ex &);
	template<class T> friend bool is_a(const ex &);
	template<class T> friend bool is_exactly_a(const ex &);

// member functions
	
	// default ctor, dtor, copy ctor, assignment operator and helpers
public:
	ex();
	~ex();
	ex(const ex & other);
	ex & operator=(const ex & other);
	// other ctors
public:
	ex(const basic & other);
	ex(int i);
	ex(unsigned int i);
	ex(long i);
	ex(unsigned long i);
	ex(double const d);

	/** Construct ex from string and a list of symbols. The input grammar is
	 *  similar to the GiNaC output format. All symbols and indices to be used
	 *  in the expression must be specified in a lst in the second argument.
	 *  Undefined symbols and other parser errors will throw an exception. */
	ex(const std::string &s, const ex &l);
	
public:
	// Iterators
	class const_iterator : public std::iterator<std::random_access_iterator_tag, ex, ptrdiff_t, const ex *, const ex &>
	{
		friend class ex;

	public:
		const_iterator() {}
		const_iterator(const basic *bp_, size_t i_) : bp(bp_), i(i_) {}

		bool operator==(const const_iterator &other) const
		{
			return bp == other.bp && i == other.i;
		}

		bool operator!=(const const_iterator &other) const
		{
			return !(*this == other);
		}

		bool operator<(const const_iterator &other) const
		{
			return i < other.i;
		}

		bool operator>(const const_iterator &other) const
		{
			return other < *this;
		}

		bool operator<=(const const_iterator &other) const
		{
			return !(other < *this);
		}

		bool operator>=(const const_iterator &other) const
		{
			return !(*this < other);
		}

		// This should return an ex&, but that would be a reference to a
		// temporary value
		ex operator*() const
		{
			return bp->op(i);
		}

#if 0
		// How do we make this work in the context of the "reference to
		// temporary" problem? Return an auto_ptr?
		pointer operator->() const
		{
			return &(operator*());
		}
#endif

		const_iterator &operator++()
		{
			++i;
			return *this;
		}

		const_iterator operator++(int)
		{
			const_iterator tmp = *this;
			++i;
			return tmp;
		}

		const_iterator &operator+=(difference_type n)
		{
			i += n;
			return *this;
		}

		const_iterator operator+(difference_type n) const
		{
			return const_iterator(bp, i + n);
		}

		inline friend const_iterator operator+(difference_type n, const const_iterator &it)
		{
			return const_iterator(it.bp, it.i + n);
		}

		const_iterator &operator--()
		{
			--i;
			return *this;
		}

		const_iterator operator--(int)
		{
			const_iterator tmp = *this;
			--i;
			return tmp;
		}

		const_iterator &operator-=(difference_type n)
		{
			i -= n;
			return *this;
		}

		const_iterator operator-(difference_type n) const
		{
			return const_iterator(bp, i - n);
		}

		inline friend difference_type operator-(const const_iterator &lhs, const const_iterator &rhs)
		{
			return lhs.i - rhs.i;
		}

		reference operator[](difference_type n) const
		{
		}

	protected:
		const basic *bp;
		size_t i;
	};

	const_iterator begin() const { return const_iterator(bp, 0); }
	const_iterator end() const { return const_iterator(bp, bp->nops()); }

#if 0
	// This doesn't work because of the "reference to temporary" problem
	// in operator*()
	typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
	const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
	const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
#endif
	
	// non-virtual functions in this class
public:
	/** Efficiently swap the contents of two expressions. */
	void swap(ex & other)
	{
		GINAC_ASSERT(bp!=0);
		GINAC_ASSERT(bp->flags & status_flags::dynallocated);
		GINAC_ASSERT(other.bp!=0);
		GINAC_ASSERT(other.bp->flags & status_flags::dynallocated);
	
		basic * tmpbp = bp;
		bp = other.bp;
		other.bp = tmpbp;
	}

	void print(const print_context & c, unsigned level = 0) const;
	void dbgprint(void) const;
	void dbgprinttree(void) const;
	bool info(unsigned inf) const { return bp->info(inf); }
	size_t nops() const { return bp->nops(); }
	ex expand(unsigned options=0) const;
	bool has(const ex & pattern) const { return bp->has(pattern); }
	ex map(map_function & f) const { return bp->map(f); }
	ex map(ex (*f)(const ex & e)) const;
	bool find(const ex & pattern, lst & found) const;
	int degree(const ex & s) const { return bp->degree(s); }
	int ldegree(const ex & s) const { return bp->ldegree(s); }
	ex coeff(const ex & s, int n = 1) const { return bp->coeff(s, n); }
	ex lcoeff(const ex & s) const { return coeff(s, degree(s)); }
	ex tcoeff(const ex & s) const { return coeff(s, ldegree(s)); }
	ex numer(void) const;
	ex denom(void) const;
	ex numer_denom(void) const;
	ex unit(const symbol &x) const;
	ex content(const symbol &x) const;
	numeric integer_content(void) const;
	ex primpart(const symbol &x) const;
	ex primpart(const symbol &x, const ex &cont) const;
	ex normal(int level = 0) const;
	ex to_rational(lst &repl_lst) const;
	ex to_polynomial(lst &repl_lst) const;
	ex smod(const numeric &xi) const { return bp->smod(xi); }
	numeric max_coefficient(void) const;
	ex collect(const ex & s, bool distributed = false) const { return bp->collect(s, distributed); }
	ex eval(int level = 0) const { return bp->eval(level); }
	ex evalf(int level = 0) const { return bp->evalf(level); }
	ex evalm(void) const { return bp->evalm(); }
	ex diff(const symbol & s, unsigned nth = 1) const;
	ex series(const ex & r, int order, unsigned options = 0) const;
	bool match(const ex & pattern) const;
	bool match(const ex & pattern, lst & repl_lst) const { return bp->match(pattern, repl_lst); }
	ex subs(const lst & ls, const lst & lr, unsigned options = 0) const { return bp->subs(ls, lr, options); }
	ex subs(const ex & e, unsigned options = 0) const { return bp->subs(e, options); }
	exvector get_free_indices(void) const { return bp->get_free_indices(); }
	ex simplify_indexed(void) const;
	ex simplify_indexed(const scalar_products & sp) const;
	ex symmetrize(void) const;
	ex symmetrize(const lst & l) const;
	ex antisymmetrize(void) const;
	ex antisymmetrize(const lst & l) const;
	ex symmetrize_cyclic(void) const;
	ex symmetrize_cyclic(const lst & l) const;
	ex eval_ncmul(const exvector & v) const { return bp->eval_ncmul(v); }
	ex op(size_t i) const { return bp->op(i); }
	ex operator[](const ex & index) const { return (*bp)[index]; }
	ex operator[](size_t i) const { return (*bp)[i]; }
	ex & let_op(size_t i);
	ex & operator[](const ex & index);
	ex & operator[](size_t i);
	ex lhs(void) const;
	ex rhs(void) const;
	int compare(const ex & other) const;
	bool is_equal(const ex & other) const;
	bool is_zero(void) const { extern const ex _ex0; return is_equal(_ex0); }
	
	unsigned return_type(void) const { return bp->return_type(); }
	unsigned return_type_tinfo(void) const { return bp->return_type_tinfo(); }
	unsigned gethash(void) const { return bp->gethash(); }
private:
	void construct_from_basic(const basic & other);
	void construct_from_int(int i);
	void construct_from_uint(unsigned int i);
	void construct_from_long(long i);
	void construct_from_ulong(unsigned long i);
	void construct_from_double(double d);
	void construct_from_string_and_lst(const std::string &s, const ex &l);
	void makewriteable();

#ifdef OBSCURE_CINT_HACK
public:
	static bool last_created_or_assigned_bp_can_be_converted_to_ex(void)
	{
		if (last_created_or_assigned_bp==0) return false;
		if ((last_created_or_assigned_bp->flags &
			 status_flags::dynallocated)==0) return false;
		if ((last_created_or_assigned_bp->flags &
			 status_flags::evaluated)==0) return false;
		return true;
	}
protected:
	void update_last_created_or_assigned_bp(void)
	{
		if (last_created_or_assigned_bp!=0) {
			if (--last_created_or_assigned_bp->refcount == 0) {
				delete last_created_or_assigned_bp;
			}
		}
		last_created_or_assigned_bp = bp;
		++last_created_or_assigned_bp->refcount;
		last_created_or_assigned_exp = (long)(void *)(this);
	}
#endif // def OBSCURE_CINT_HACK

// member variables

private:
	basic *bp;      ///< pointer to basic object managed by this
#ifdef OBSCURE_CINT_HACK
public:
	static basic * last_created_or_assigned_bp;
	static basic * dummy_bp;
	static long last_created_or_assigned_exp;
#endif // def OBSCURE_CINT_HACK
};


// performance-critical inlined method implementations

inline
ex::ex()
{
	extern const class numeric *_num0_p;
	bp = (basic*)_num0_p;
	GINAC_ASSERT(bp!=0);
	GINAC_ASSERT(bp->flags & status_flags::dynallocated);
	++bp->refcount;
#ifdef OBSCURE_CINT_HACK
	update_last_created_or_assigned_bp();
#endif // def OBSCURE_CINT_HACK
}

inline
ex::~ex()
{
	GINAC_ASSERT(bp!=0);
	GINAC_ASSERT(bp->flags & status_flags::dynallocated);
	if (--bp->refcount == 0)
		delete bp;
}

inline
ex::ex(const ex & other) : bp(other.bp)
{
	GINAC_ASSERT(bp!=0);
	GINAC_ASSERT((bp->flags) & status_flags::dynallocated);
	++bp->refcount;
#ifdef OBSCURE_CINT_HACK
	update_last_created_or_assigned_bp();
#endif // def OBSCURE_CINT_HACK
}

inline
ex & ex::operator=(const ex & other)
{
	GINAC_ASSERT(bp!=0);
	GINAC_ASSERT(bp->flags & status_flags::dynallocated);
	GINAC_ASSERT(other.bp!=0);
	GINAC_ASSERT(other.bp->flags & status_flags::dynallocated);
	// NB: must first increment other.bp->refcount, since other might be *this.
	++other.bp->refcount;
	if (--bp->refcount==0)
		delete bp;
	bp = other.bp;
#ifdef OBSCURE_CINT_HACK
	update_last_created_or_assigned_bp();
#endif // def OBSCURE_CINT_HACK
	return *this;
}

inline
ex::ex(const basic & other)
{
	construct_from_basic(other);
#ifdef OBSCURE_CINT_HACK
	update_last_created_or_assigned_bp();
#endif // def OBSCURE_CINT_HACK
}

inline
ex::ex(int i)
{
	construct_from_int(i);
#ifdef OBSCURE_CINT_HACK
	update_last_created_or_assigned_bp();
#endif // def OBSCURE_CINT_HACK
}

inline
ex::ex(unsigned int i)
{
	construct_from_uint(i);
#ifdef OBSCURE_CINT_HACK
	update_last_created_or_assigned_bp();
#endif // def OBSCURE_CINT_HACK
}

inline
ex::ex(long i)
{
	construct_from_long(i);
#ifdef OBSCURE_CINT_HACK
	update_last_created_or_assigned_bp();
#endif // def OBSCURE_CINT_HACK
}

inline
ex::ex(unsigned long i)
{
	construct_from_ulong(i);
#ifdef OBSCURE_CINT_HACK
	update_last_created_or_assigned_bp();
#endif // def OBSCURE_CINT_HACK
}

inline
ex::ex(double const d)
{
	construct_from_double(d);
#ifdef OBSCURE_CINT_HACK
	update_last_created_or_assigned_bp();
#endif // def OBSCURE_CINT_HACK
}

inline
ex::ex(const std::string &s, const ex &l)
{
	construct_from_string_and_lst(s, l);
#ifdef OBSCURE_CINT_HACK
	update_last_created_or_assigned_bp();
#endif // def OBSCURE_CINT_HACK
}

inline
int ex::compare(const ex & other) const
{
	GINAC_ASSERT(bp!=0);
	GINAC_ASSERT(other.bp!=0);
	if (bp==other.bp)  // trivial case: both expressions point to same basic
		return 0;
	return bp->compare(*other.bp);
}

inline
bool ex::is_equal(const ex & other) const
{
	GINAC_ASSERT(bp!=0);
	GINAC_ASSERT(other.bp!=0);
	if (bp==other.bp)  // trivial case: both expressions point to same basic
		return true;
	return bp->is_equal(*other.bp);
}


// utility functions

/** Compare two objects of class quickly without doing a deep tree traversal.
 *  @return "true" if they are equal
 *          "false" if equality cannot be established quickly (e1 and e2 may
 *          still be equal, in this case. */
inline bool are_ex_trivially_equal(const ex &e1, const ex &e2)
{
	return e1.bp == e2.bp;
}

// wrapper functions around member functions
inline size_t nops(const ex & thisex)
{ return thisex.nops(); }

inline ex expand(const ex & thisex, unsigned options = 0)
{ return thisex.expand(options); }

inline bool has(const ex & thisex, const ex & pattern)
{ return thisex.has(pattern); }

inline bool find(const ex & thisex, const ex & pattern, lst & found)
{ return thisex.find(pattern, found); }

inline int degree(const ex & thisex, const ex & s)
{ return thisex.degree(s); }

inline int ldegree(const ex & thisex, const ex & s)
{ return thisex.ldegree(s); }

inline ex coeff(const ex & thisex, const ex & s, int n=1)
{ return thisex.coeff(s, n); }

inline ex numer(const ex & thisex)
{ return thisex.numer(); }

inline ex denom(const ex & thisex)
{ return thisex.denom(); }

inline ex numer_denom(const ex & thisex)
{ return thisex.numer_denom(); }

inline ex normal(const ex & thisex, int level=0)
{ return thisex.normal(level); }

inline ex to_rational(const ex & thisex, lst & repl_lst)
{ return thisex.to_rational(repl_lst); }

inline ex to_polynomial(const ex & thisex, lst & repl_lst)
{ return thisex.to_polynomial(repl_lst); }

inline ex collect(const ex & thisex, const ex & s, bool distributed = false)
{ return thisex.collect(s, distributed); }

inline ex eval(const ex & thisex, int level = 0)
{ return thisex.eval(level); }

inline ex evalf(const ex & thisex, int level = 0)
{ return thisex.evalf(level); }

inline ex evalm(const ex & thisex)
{ return thisex.evalm(); }

inline ex diff(const ex & thisex, const symbol & s, unsigned nth = 1)
{ return thisex.diff(s, nth); }

inline ex series(const ex & thisex, const ex & r, int order, unsigned options = 0)
{ return thisex.series(r, order, options); }

inline bool match(const ex & thisex, const ex & pattern, lst & repl_lst)
{ return thisex.match(pattern, repl_lst); }

inline ex subs(const ex & thisex, const ex & e, unsigned options = 0)
{ return thisex.subs(e, options); }

inline ex subs(const ex & thisex, const lst & ls, const lst & lr, unsigned options = 0)
{ return thisex.subs(ls, lr, options); }

inline ex simplify_indexed(const ex & thisex)
{ return thisex.simplify_indexed(); }

inline ex simplify_indexed(const ex & thisex, const scalar_products & sp)
{ return thisex.simplify_indexed(sp); }

inline ex symmetrize(const ex & thisex)
{ return thisex.symmetrize(); }

inline ex symmetrize(const ex & thisex, const lst & l)
{ return thisex.symmetrize(l); }

inline ex antisymmetrize(const ex & thisex)
{ return thisex.antisymmetrize(); }

inline ex antisymmetrize(const ex & thisex, const lst & l)
{ return thisex.antisymmetrize(l); }

inline ex symmetrize_cyclic(const ex & thisex)
{ return thisex.symmetrize_cyclic(); }

inline ex symmetrize_cyclic(const ex & thisex, const lst & l)
{ return thisex.symmetrize_cyclic(l); }

inline ex op(const ex & thisex, size_t i)
{ return thisex.op(i); }

inline ex lhs(const ex & thisex)
{ return thisex.lhs(); }

inline ex rhs(const ex & thisex)
{ return thisex.rhs(); }

inline bool is_zero(const ex & thisex)
{ return thisex.is_zero(); }

inline void swap(ex & e1, ex & e2)
{ e1.swap(e2); }

/* Function objects for STL sort() etc. */
struct ex_is_less : public std::binary_function<ex, ex, bool> {
	bool operator() (const ex &lh, const ex &rh) const { return lh.compare(rh) < 0; }
};

struct ex_is_equal : public std::binary_function<ex, ex, bool> {
	bool operator() (const ex &lh, const ex &rh) const { return lh.is_equal(rh); }
};

struct op0_is_equal : public std::binary_function<ex, ex, bool> {
	bool operator() (const ex &lh, const ex &rh) const { return lh.op(0).is_equal(rh.op(0)); }
};

struct ex_swap : public std::binary_function<ex, ex, void> {
	void operator() (ex &lh, ex &rh) const { lh.swap(rh); }
};


/* Convert function pointer to function object suitable for map(). */
class pointer_to_map_function : public map_function {
protected:
	ex (*ptr)(const ex &);
public:
	explicit pointer_to_map_function(ex (*x)(const ex &)) : ptr(x) {}
	ex operator()(const ex & e) { return ptr(e); }
};

template<class T1>
class pointer_to_map_function_1arg : public map_function {
protected:
	ex (*ptr)(const ex &, T1);
	T1 arg1;
public:
	explicit pointer_to_map_function_1arg(ex (*x)(const ex &, T1), T1 a1) : ptr(x), arg1(a1) {}
	ex operator()(const ex & e) { return ptr(e, arg1); }
};

template<class T1, class T2>
class pointer_to_map_function_2args : public map_function {
protected:
	ex (*ptr)(const ex &, T1, T2);
	T1 arg1;
	T2 arg2;
public:
	explicit pointer_to_map_function_2args(ex (*x)(const ex &, T1, T2), T1 a1, T2 a2) : ptr(x), arg1(a1), arg2(a2) {}
	ex operator()(const ex & e) { return ptr(e, arg1, arg2); }
};

template<class T1, class T2, class T3>
class pointer_to_map_function_3args : public map_function {
protected:
	ex (*ptr)(const ex &, T1, T2, T3);
	T1 arg1;
	T2 arg2;
	T3 arg3;
public:
	explicit pointer_to_map_function_3args(ex (*x)(const ex &, T1, T2, T3), T1 a1, T2 a2, T3 a3) : ptr(x), arg1(a1), arg2(a2), arg3(a3) {}
	ex operator()(const ex & e) { return ptr(e, arg1, arg2, arg3); }
};

inline ex ex::map(ex (*f)(const ex & e)) const
{
	pointer_to_map_function fcn(f);
	return bp->map(fcn);
}

// convenience type checker template functions

/** Check if ex is a handle to a T, including base classes. */
template <class T>
inline bool is_a(const ex &obj)
{
	return is_a<T>(*obj.bp);
}

/** Check if ex is a handle to a T, not including base classes. */
template <class T>
inline bool is_exactly_a(const ex &obj)
{
	return is_exactly_a<T>(*obj.bp);
}

/** Return a reference to the basic-derived class T object embedded in an
 *  expression.  This is fast but unsafe: the result is undefined if the
 *  expression does not contain a T object at its top level.  Hence, you
 *  should generally check the type of e first.
 *
 *  @param e expression
 *  @return reference to object of class T
 *  @see is_exactly_a<class T>() */
template <class T>
inline const T &ex_to(const ex &e)
{
	GINAC_ASSERT(is_a<T>(e));
	return static_cast<const T &>(*e.bp);
}

} // namespace GiNaC


// Specializations of Standard Library algorithms
namespace std {

/** Specialization of std::swap() for ex objects. */
template <>
inline void swap(GiNaC::ex &a, GiNaC::ex &b)
{
	a.swap(b);
}

/** Specialization of std::iter_swap() for vector<ex> iterators. */
template <>
inline void iter_swap(vector<GiNaC::ex>::iterator i1, vector<GiNaC::ex>::iterator i2)
{
	i1->swap(*i2);
}

} // namespace std

#endif // ndef __GINAC_EX_H__
