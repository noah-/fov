#ifndef lint
#ifdef __GNUC__
#define ATTRIBUTE(attrs) __attribute__(attrs)
#else
#define ATTRIBUTE(attrs)
#endif
static char Rcs_Id[] ATTRIBUTE((used)) =
    "$Id: mtwist.c,v 1.28 2014-01-23 21:11:42-08 geoff Exp $";
#endif


#ifdef _WIN32
#undef WIN32
#define WIN32
#endif

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef WIN32
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif

#define MT_INLINE
#define MT_EXTERN
#undef MT_GENERATE_CODE_IN_HEADER
#define MT_GENERATE_CODE_IN_HEADER 1

#include "mtwist.h"

static uint32_t		mts_devseed(mt_state* state, char* seed_dev);

#define RECURRENCE_OFFSET 397
#define MATRIX_A	0x9908b0df

#define BIT_WIDTH	32

#define UPPER_MASK	0x80000000
#define LOWER_MASK	0x7fffffff

#define COMBINE_BITS(x, y) \
			(((x) & UPPER_MASK) | ((y) & LOWER_MASK))


#define MATRIX_MULTIPLY(original, new) \
			((original) ^ ((new) >> 1) \
			  ^ matrix_decider[(new) & 0x1])

#define KNUTH_MULTIPLIER_OLD \
			69069

#define KNUTH_MULTIPLIER_NEW \
			1812433253ul
#define KNUTH_SHIFT	30


#define DEFAULT_SEED32_OLD \
			4357
#define DEFAULT_SEED32_NEW \
			5489ul

#define DEVRANDOM	"/dev/random"
#define DEVURANDOM	"/dev/urandom"

mt_state		mt_default_state;


double			mt_32_to_double;

double			mt_64_to_double;

static uint32_t	matrix_decider[2] =
			  {0x0, MATRIX_A};


void mts_mark_initialized(
    mt_state*		state)
    {
    int			i;

    mt_32_to_double = 1.0;
    for (i = 0;  i < BIT_WIDTH;  i++)
	mt_32_to_double /= 2.0;
    mt_64_to_double = mt_32_to_double;
    for (i = 0;  i < BIT_WIDTH;  i++)
	mt_64_to_double /= 2.0;

    state->initialized = 1;
    }

void mts_seed32(
    mt_state*		state,
    uint32_t		seed)
    {
    int			i;

    if (seed == 0)
	seed = DEFAULT_SEED32_OLD;

    state->statevec[MT_STATE_SIZE - 1] = seed & 0xffffffff;
    for (i = MT_STATE_SIZE - 2;  i >= 0;  i--)
        state->statevec[i] =
          (KNUTH_MULTIPLIER_OLD * state->statevec[i + 1]) & 0xffffffff;

    state->stateptr = MT_STATE_SIZE;
    mts_mark_initialized(state);


    mts_refresh(state);
    }


void mts_seed32new(
    mt_state*		state,
    uint32_t		seed)
    {
    int			i;
    uint32_t		nextval;

    state->statevec[MT_STATE_SIZE - 1] = seed & 0xffffffffUL;
    for (i = MT_STATE_SIZE - 2;  i >= 0;  i--)
	{
	nextval = state->statevec[i + 1] >> KNUTH_SHIFT;
	nextval ^= state->statevec[i + 1];
	nextval *= KNUTH_MULTIPLIER_NEW;
	nextval += (MT_STATE_SIZE - 1) - i;
	state->statevec[i] = nextval & 0xffffffffUL;
	}

    state->stateptr = MT_STATE_SIZE;
    mts_mark_initialized(state);

    mts_refresh(state);
    }


void mts_seedfull(
    mt_state*		state,
    uint32_t		seeds[MT_STATE_SIZE])

    {
    int			had_nz = 0;
    int			i;

    for (i = 0;  i < MT_STATE_SIZE;  i++)
        {
        if (seeds[i] != 0)
	    had_nz = 1;
        state->statevec[MT_STATE_SIZE - i - 1] = seeds[i];
	}

    if (!had_nz)
	{
    	abort();
	}

    state->stateptr = MT_STATE_SIZE;
    mts_mark_initialized(state);
    }

uint32_t mts_seed(
    mt_state*		state)
    {
    return mts_devseed(state, DEVURANDOM);
    }

uint32_t mts_goodseed(
    mt_state*		state)
    {
    return mts_devseed(state, DEVRANDOM);
    }

static uint32_t mts_devseed(
    mt_state*		state,
    char*		seed_dev)
    {
    int			bytesread;
    int			nextbyte;
    FILE*		ranfile;
    union
	{
	char		ranbuffer[sizeof (uint32_t)];

	uint32_t	randomvalue;
	}
			randomunion;
#ifdef WIN32
    struct _timeb	tb;
#else
    struct timeval	tv;
#endif

    ranfile = fopen(seed_dev, "rb");

    if (ranfile == NULL)
	ranfile = fopen(DEVRANDOM, "rb");
    if (ranfile != NULL)
	{
	setbuf(ranfile, NULL);
	for (nextbyte = 0;
	  nextbyte < (int)sizeof randomunion.ranbuffer;
	  nextbyte += bytesread)
	    {
	    bytesread = fread(&randomunion.ranbuffer[nextbyte], (size_t)1,
	      sizeof randomunion.ranbuffer - nextbyte, ranfile);
	    if (bytesread == 0)
		break;
	    }
	fclose(ranfile);
	if (nextbyte == sizeof randomunion.ranbuffer)
	    {
	    mts_seed32new(state, randomunion.randomvalue);
	    return randomunion.randomvalue;
	    }
	}

#ifdef WIN32
    (void) _ftime (&tb);
#else
    (void) gettimeofday (&tv, NULL);
#endif
#ifdef WIN32
    randomunion.randomvalue = tb.time * 1000 + tb.millitm;
#else
    randomunion.randomvalue = tv.tv_sec * 1000000 + tv.tv_usec;
#endif
    mts_seed32new(state, randomunion.randomvalue);
    return randomunion.randomvalue;
    }

void mts_bestseed(
    mt_state*		state)
    {
    int			bytesread;
    int			nextbyte;
    FILE*		ranfile;

    ranfile = fopen("/dev/random", "rb");
    if (ranfile == NULL)
	{
	mts_goodseed(state);
	return;
	}

    for (nextbyte = 0;
      nextbyte < (int)sizeof state->statevec;
      nextbyte += bytesread)
	{
	bytesread = fread((char *)&state->statevec[0] + nextbyte, (size_t)1,
	  sizeof state->statevec - nextbyte, ranfile);
	if (bytesread == 0)
	    {
	    fclose(ranfile);
	    mts_goodseed(state);
	    return;
	    }
	}
    fclose(ranfile);
    }

void mts_refresh(
    register mt_state*	state)
    {
    register int	i;
    register uint32_t*
			state_ptr;
    register uint32_t
			value1;
    register uint32_t
			value2;

    if (!state->initialized)
	{
	mts_seed32(state, DEFAULT_SEED32_OLD);
	return;
	}

    state_ptr = &state->statevec[MT_STATE_SIZE - 1];
    value1 = *state_ptr;
    for (i = (MT_STATE_SIZE - RECURRENCE_OFFSET) / 2;  --i >= 0;  )
	{
	state_ptr -= 2;
	value2 = state_ptr[1];
	value1 = COMBINE_BITS(value1, value2);
	state_ptr[2] =
	  MATRIX_MULTIPLY(state_ptr[-RECURRENCE_OFFSET + 2], value1);
	value1 = state_ptr[0];
	value2 = COMBINE_BITS(value2, value1);
	state_ptr[1] =
	  MATRIX_MULTIPLY(state_ptr[-RECURRENCE_OFFSET + 1], value2);
	}
    value2 = *--state_ptr;
    value1 = COMBINE_BITS(value1, value2);
    state_ptr[1] =
      MATRIX_MULTIPLY(state_ptr[-RECURRENCE_OFFSET + 1], value1);

    for (i = (RECURRENCE_OFFSET - 1) / 2;  --i >= 0;  )
	{
	state_ptr -= 2;
	value1 = state_ptr[1];
	value2 = COMBINE_BITS(value2, value1);
	state_ptr[2] =
	  MATRIX_MULTIPLY(state_ptr[MT_STATE_SIZE - RECURRENCE_OFFSET + 2],
	    value2);
	value2 = state_ptr[0];
	value1 = COMBINE_BITS(value1, value2);
	state_ptr[1] =
	  MATRIX_MULTIPLY(state_ptr[MT_STATE_SIZE - RECURRENCE_OFFSET + 1],
	    value1);
	}

    value1 = COMBINE_BITS(value2, state->statevec[MT_STATE_SIZE - 1]);
    *state_ptr =
      MATRIX_MULTIPLY(state_ptr[MT_STATE_SIZE - RECURRENCE_OFFSET], value1);

    state->stateptr = MT_STATE_SIZE;
    }

int mts_savestate(
    FILE*		statefile,
    mt_state*		state)
    {
    int			i;

    if (!state->initialized)
	mts_seed32(state, DEFAULT_SEED32_OLD);

    if (state->stateptr < 0  ||  state->stateptr > MT_STATE_SIZE)
	{
	fprintf(stderr,
	  "Mtwist internal: Trying to write invalid state pointer %d\n",
	  state->stateptr);
	mts_refresh(state);
	}

    for (i = MT_STATE_SIZE;  --i >= 0;  )
	{
	if (fprintf(statefile, "%" PRIu32 " ", state->statevec[i]) < 0)
	    return 0;
	}

    if (fprintf(statefile, "%d\n", state->stateptr) < 0)
	return 0;

    return 1;
    }

int mts_loadstate(
    FILE*		statefile,
    mt_state*		state)
    {
    int			i;

    state->initialized = state->stateptr = 0;

    for (i = MT_STATE_SIZE;  --i >= 0;  )
	{
	if (fscanf(statefile, "%" SCNu32, &state->statevec[i]) != 1)
	    return 0;
	}

    if (fscanf(statefile, "%d", &state->stateptr) != 1)
	return 0;

    if (state->stateptr < 0  ||  state->stateptr > MT_STATE_SIZE)
	{
	state->stateptr = 0;
	return 0;
	}

    mts_mark_initialized(state);

    return 1;
    }

void mt_seed32(
    uint32_t		seed)
    {
    mts_seed32(&mt_default_state, seed);
    }

void mt_seed32new(
    uint32_t		seed)
    {
    mts_seed32new(&mt_default_state, seed);
    }

void mt_seedfull(
    uint32_t		seeds[MT_STATE_SIZE])
    {
    mts_seedfull(&mt_default_state, seeds);
    }

uint32_t mt_seed(void)
    {
    return mts_seed(&mt_default_state);
    }

uint32_t mt_goodseed(void)
    {
    return mts_goodseed(&mt_default_state);
    }

void mt_bestseed(void)
    {
    mts_bestseed(&mt_default_state);
    }

mt_state* mt_getstate(void)
    {
    return &mt_default_state;
    }

int mt_savestate(
    FILE*		statefile)
    {
    return mts_savestate(statefile, &mt_default_state);
    }


int mt_loadstate(
    FILE*		statefile)
    {
    return mts_loadstate(statefile, &mt_default_state);
    }
