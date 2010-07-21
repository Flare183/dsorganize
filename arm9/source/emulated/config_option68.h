/* Configured options for sc68. */

#ifndef _SC68_CONFIG_OPTION68_H_
#define _SC68_CONFIG_OPTION68_H_

#ifdef HAVE_CONFIG_H

#define ISTREAM_NO_FD 1
#define ISTREAM_NO_FILE 1
#define ISTREAM_NO_CURL 1

#ifndef REPLAY_RATE_MIN
# define REPLAY_RATE_MIN   25
#endif

#ifndef SAMPLING_RATE_MIN
# define SAMPLING_RATE_MIN 44100
#endif

#ifndef SAMPLING_RATE_MAX
# define SAMPLING_RATE_MAX 44100
#endif

#ifndef SAMPLING_RATE_DEF
# define SAMPLING_RATE_DEF 44100
#endif

#endif /* #ifndef HAVE_CONFIG_H */

/* Fallback */
#ifndef NO_FALLBACK_CONFIG

#ifndef REPLAY_RATE_MIN
# define REPLAY_RATE_MIN   25
#endif

#ifndef SAMPLING_RATE_MIN
# define SAMPLING_RATE_MIN 44100
#endif

#ifndef SAMPLING_RATE_MAX
# define SAMPLING_RATE_MAX 44100
#endif

#ifndef SAMPLING_RATE_DEF
# define SAMPLING_RATE_DEF 44100
#endif

#endif /* #ifndef NO_FALLBACK_CONFIG */

#endif /* #ifndef _SC68_CONFIG_OPTION68_H_ */
