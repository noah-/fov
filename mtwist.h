#ifndef MTWIST_H
#define MTWIST_H


#include <stdio.h>
#ifdef __cplusplus
#include <iostream>
#endif

#define __STDC_LIMIT_MACROS
#include <stdint.h>

#define MT_STATE_SIZE	624

typedef struct
    {
    uint32_t		statevec[MT_STATE_SIZE];

    int			stateptr;
    int			initialized;
    }
			mt_state;

#ifdef __cplusplus
extern "C"
    {
#endif

extern void		mts_mark_initialized(mt_state* state);

extern void		mts_seed32(mt_state* state, uint32_t seed);

extern void		mts_seed32new(mt_state* state, uint32_t seed);

extern void		mts_seedfull(mt_state* state,
			  uint32_t seeds[MT_STATE_SIZE]);

extern uint32_t		mts_seed(mt_state* state);

extern uint32_t		mts_goodseed(mt_state* state);
extern void		mts_bestseed(mt_state* state);
extern void		mts_refresh(mt_state* state);

extern int		mts_savestate(FILE* statefile, mt_state* state);
extern int		mts_loadstate(FILE* statefile, mt_state* state);
extern void		mt_seed32(uint32_t seed);

extern void		mt_seed32new(uint32_t seed);

extern void		mt_seedfull(uint32_t seeds[MT_STATE_SIZE]);

extern uint32_t		mt_seed(void);
extern uint32_t		mt_goodseed(void);
extern void		mt_bestseed(void);

extern mt_state*	mt_getstate(void);

extern int		mt_savestate(FILE* statefile);

extern int		mt_loadstate(FILE* statefile);


#ifdef __cplusplus
    }
#endif
#ifdef __cplusplus
#endif

extern uint32_t		mts_lrand(mt_state* state);

#ifdef UINT64_MAX
extern uint64_t		mts_llrand(mt_state* state);

#endif
extern double		mts_drand(mt_state* state);

extern double		mts_ldrand(mt_state* state);


extern uint32_t		mt_lrand(void);
#ifdef UINT64_MAX
extern uint64_t		mt_llrand(void);

#endif
extern double		mt_drand(void);

extern double		mt_ldrand(void);

#define MT_TEMPERING_MASK_B 0x9d2c5680
#define MT_TEMPERING_MASK_C 0xefc60000
#define MT_TEMPERING_SHIFT_U(y) \
			(y >> 11)
#define MT_TEMPERING_SHIFT_S(y) \
			(y << 7)
#define MT_TEMPERING_SHIFT_T(y) \
			(y << 15)
#define MT_TEMPERING_SHIFT_L(y) \
			(y >> 18)

#define MT_PRE_TEMPER(value)						\
    do									\
	{								\
	value ^= MT_TEMPERING_SHIFT_U(value);				\
	value ^= MT_TEMPERING_SHIFT_S(value) & MT_TEMPERING_MASK_B;	\
	value ^= MT_TEMPERING_SHIFT_T(value) & MT_TEMPERING_MASK_C;	\
	}								\
	while (0)
#define MT_FINAL_TEMPER(value) \
			((value) ^ MT_TEMPERING_SHIFT_L(value))
#define MT_TEMPER(value)						\
    do									\
	{								\
	value ^= MT_TEMPERING_SHIFT_U(value);				\
	value ^= MT_TEMPERING_SHIFT_S(value) & MT_TEMPERING_MASK_B;	\
	value ^= MT_TEMPERING_SHIFT_T(value) & MT_TEMPERING_MASK_C;	\
	value ^= MT_TEMPERING_SHIFT_L(value);				\
	}								\
	while (0)

extern mt_state		mt_default_state;

extern double		mt_32_to_double;

extern double		mt_64_to_double;


#ifndef MT_EXTERN
#ifdef __cplusplus
#define MT_EXTERN
#else
#define MT_EXTERN	extern
#endif
#endif


#ifndef MT_INLINE
#define MT_INLINE	inline
#endif

#ifndef MT_GENERATE_CODE_IN_HEADER
#ifdef __GNUC__
#define MT_GENERATE_CODE_IN_HEADER 1
#endif
#if defined(__INTEL_COMPILER)  ||  defined(_MSC_VER)
#define MT_GENERATE_CODE_IN_HEADER 0
#endif
#endif

#if MT_GENERATE_CODE_IN_HEADER

MT_EXTERN MT_INLINE uint32_t mts_lrand(
    register mt_state*	state)
    {
    register uint32_t	random_value;

    if (state->stateptr <= 0)
	mts_refresh(state);

    random_value = state->statevec[--state->stateptr];
    MT_PRE_TEMPER(random_value);
    return MT_FINAL_TEMPER(random_value);
    }

#ifdef UINT64_MAX

MT_EXTERN MT_INLINE uint64_t mts_llrand(
    register mt_state*	state)
    {
    register uint32_t	random_value_1;
    register uint32_t	random_value_2;


    if (--state->stateptr <= 0)
	{
	if (state->stateptr < 0)
	    {
	    mts_refresh(state);
	    random_value_1 = state->statevec[--state->stateptr];
	    }
	else
	    {
	    random_value_1 = state->statevec[state->stateptr];
	    mts_refresh(state);
	    }
	}
    else
	random_value_1 = state->statevec[state->stateptr];

    MT_TEMPER(random_value_1);

    random_value_2 = state->statevec[--state->stateptr];
    MT_PRE_TEMPER(random_value_2);

    return ((uint64_t) random_value_1 << 32)
      | (uint64_t) MT_FINAL_TEMPER(random_value_2);
    }
#endif

MT_EXTERN MT_INLINE double mts_drand(
    register mt_state*	state)
    {
    register uint32_t	random_value;

    if (state->stateptr <= 0)
	mts_refresh(state);

    random_value = state->statevec[--state->stateptr];
    MT_TEMPER(random_value);

    return random_value * mt_32_to_double;
    }

MT_EXTERN MT_INLINE double mts_ldrand(
    register mt_state*	state)
    {
#ifdef UINT64_MAX
    uint64_t		final_value;
#endif
    register uint32_t	random_value_1;
    register uint32_t	random_value_2;

    if (--state->stateptr <= 0)
	{
	if (state->stateptr < 0)
	    {
	    mts_refresh(state);
	    random_value_1 = state->statevec[--state->stateptr];
	    }
	else
	    {
	    random_value_1 = state->statevec[state->stateptr];
	    mts_refresh(state);
	    }
	}
    else
	random_value_1 = state->statevec[state->stateptr];

    MT_TEMPER(random_value_1);

    random_value_2 = state->statevec[--state->stateptr];
    MT_TEMPER(random_value_2);

#ifdef UINT64_MAX
    final_value = ((uint64_t) random_value_1 << 32) | (uint64_t) random_value_2;
    return final_value * mt_64_to_double;
#else
    return random_value_1 * mt_32_to_double + random_value_2 * mt_64_to_double;
#endif
    }


MT_EXTERN MT_INLINE uint32_t mt_lrand(void)
    {
    register uint32_t	random_value;

    if (mt_default_state.stateptr <= 0)
	mts_refresh(&mt_default_state);

    random_value = mt_default_state.statevec[--mt_default_state.stateptr];
    MT_PRE_TEMPER(random_value);

    return MT_FINAL_TEMPER(random_value);
    }

#ifdef UINT64_MAX

MT_EXTERN MT_INLINE uint64_t mt_llrand(void)
    {
    register uint32_t	random_value_1;
    register uint32_t	random_value_2;


    if (--mt_default_state.stateptr <= 0)
	{
	if (mt_default_state.stateptr < 0)
	    {
	    mts_refresh(&mt_default_state);
	    random_value_1 =
	      mt_default_state.statevec[--mt_default_state.stateptr];
	    }
	else
	    {
	    random_value_1 =
	      mt_default_state.statevec[mt_default_state.stateptr];
	    mts_refresh(&mt_default_state);
	    }
	}
    else
	random_value_1 = mt_default_state.statevec[mt_default_state.stateptr];

    MT_TEMPER(random_value_1);

    random_value_2 = mt_default_state.statevec[--mt_default_state.stateptr];
    MT_PRE_TEMPER(random_value_2);

    return ((uint64_t) random_value_1 << 32)
      | (uint64_t) MT_FINAL_TEMPER(random_value_2);
    }
#endif
MT_EXTERN MT_INLINE double mt_drand(void)
    {
    register uint32_t	random_value;

    if (mt_default_state.stateptr <= 0)
	mts_refresh(&mt_default_state);

    random_value = mt_default_state.statevec[--mt_default_state.stateptr];
    MT_TEMPER(random_value);

    return random_value * mt_32_to_double;
    }

MT_EXTERN MT_INLINE double mt_ldrand(void)
    {
#ifdef UINT64_MAX
    uint64_t		final_value;
#endif
    register uint32_t	random_value_1;
    register uint32_t	random_value_2;
    if (--mt_default_state.stateptr <= 0)
	{
	if (mt_default_state.stateptr < 0)
	    {
	    mts_refresh(&mt_default_state);
	    random_value_1 =
	      mt_default_state.statevec[--mt_default_state.stateptr];
	    }
	else
	    {
	    random_value_1 =
	      mt_default_state.statevec[mt_default_state.stateptr];
	    mts_refresh(&mt_default_state);
	    }
	}
    else
	random_value_1 = mt_default_state.statevec[mt_default_state.stateptr];

    MT_TEMPER(random_value_1);

    random_value_2 = mt_default_state.statevec[--mt_default_state.stateptr];
    MT_TEMPER(random_value_2);

#ifdef UINT64_MAX
    final_value = ((uint64_t) random_value_1 << 32) | (uint64_t) random_value_2;
    return final_value * mt_64_to_double;
#else
    return random_value_1 * mt_32_to_double + random_value_2 * mt_64_to_double;
#endif
    }
#endif

#ifdef __cplusplus

class mt_prng
    {
	friend class mt_empirical_distribution;
    public:
			mt_prng(	// Default constructor
			    bool pickSeed = false)
					// True to get seed from /dev/urandom
					// ..or time
			    {
			    state.stateptr = 0;
			    state.initialized = 0;
			    if (pickSeed)
				(void)mts_seed(&state);
			    }
			mt_prng(uint32_t newseed)
					// Construct with 32-bit seeding
			    {
			    state.stateptr = 0;
			    state.initialized = 0;
			    mts_seed32(&state, newseed);
			    }
			mt_prng(uint32_t seeds[MT_STATE_SIZE])
					// Construct with full seeding
			    {
			    state.stateptr = 0;
			    state.initialized = 0;
			    mts_seedfull(&state, seeds);
			    }
			~mt_prng() { }


	void		seed32(uint32_t newseed)
					// Set 32-bit random seed
			    {
			    mts_seed32(&state, newseed);
			    }
	void		seed32new(uint32_t newseed)
					// Set 32-bit random seed
			    {
			    mts_seed32new(&state, newseed);
			    }
	void		seedfull(uint32_t seeds[MT_STATE_SIZE])
					// Set complicated random seed
			    {
			    mts_seedfull(&state, seeds);
			    }
	uint32_t	seed()		// Choose seed from random input
			    {
			    return mts_seed(&state);
			    }
	uint32_t	goodseed()	// Choose better seed from random input
			    {
			    return mts_goodseed(&state);
			    }
	void		bestseed()	// Choose best seed from random input
			    {
			    mts_bestseed(&state);
			    }
	friend std::ostream&
			operator<<(std::ostream& stream, const mt_prng& rng);
	friend std::istream&
			operator>>(std::istream& stream, mt_prng& rng);


	uint32_t	lrand()		// Generate 32-bit pseudo-random value
			    {
			    return mts_lrand(&state);
			    }
#ifdef UINT64_MAX
	uint64_t	llrand()	// Generate 64-bit pseudo-random value
			    {
			    return mts_llrand(&state);
			    }
#endif
	double		drand()		// Generate fast 32-bit floating value
			    {
			    return mts_drand(&state);
			    }
	double		ldrand()	// Generate slow 64-bit floating value
			    {
			    return mts_ldrand(&state);
			    }

		double		operator()()
			    {
			    return mts_drand(&state);
			    }
    protected:
	mt_state	state;		// Current state of the PRNG
    };

#if MT_GENERATE_CODE_IN_HEADER

MT_INLINE std::ostream& operator<<(
    std::ostream&	stream,		// Stream to save to
    const mt_prng&	rng)		// PRNG to save
    {
    for (int i = MT_STATE_SIZE;  --i >= 0;  )
	{
	if (!(stream << rng.state.statevec[i] << ' '))
	    return stream;
	}

    return stream << rng.state.stateptr;
    }


MT_INLINE std::istream& operator>>(
    std::istream&	stream,		// Stream to laod from
    mt_prng&		rng)		// PRNG to load
    {
    rng.state.initialized = rng.state.stateptr = 0;
    for (int i = MT_STATE_SIZE;  --i >= 0;  )
	{
	if (!(stream >> rng.state.statevec[i]))
	    return stream;
	}

    if (!(stream >> rng.state.stateptr))
	{
	rng.state.stateptr = 0;
	return stream;
	}


    if (rng.state.stateptr < 0  ||  rng.state.stateptr > MT_STATE_SIZE)
	{
	rng.state.stateptr = 0;
	return stream;
	}

    mts_mark_initialized(&rng.state);

    return stream;
    }
#endif
#endif

#endif
