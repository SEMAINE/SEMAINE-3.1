/* include/sent/config.h.  Generated automatically by configure.  */
/* include/sent/config.h.in.  Generated automatically from configure.in by autoheader.  */

/**
 * @file   libsent/include/sent/config.h
 *
 * <EN>
 * @brief  Environment configuration file
 *
 * This file contains symbol definitions to switch running environment
 * and configuration of this library.
 * This file was generated from config.h.in by @c configure,
 * switching each definitions according to the arguments.
 *
 * The symboles above are defined by default when running @c configure
 * with no argument.
 * To see all definition in detail, please see the source code.
 * @sa configure
 * </EN>
 * <JA>
 * @brief  �Ķ�����ե�����
 *
 * ���Υե�����ˤ�ư��Ķ��������ɽ������ܥ��������ޤޤ�Ƥ��ޤ���
 * ���Υե������ config.h.in ���� @c configure �μ¹Ԥˤ�ä�
 * ��ưŪ����������ޤ���
 *
 * �嵭�������, @c configure �����̵���Ǽ¹Ԥ����Ȥ��˥ǥե���Ȥ�
 * �������륷��ܥ�Ǥ���
 * ������ξܺ٤ˤĤ��Ƥ�, �����������ɤ�ľ�ܤ�������������
 * @sa configure
 * @sa configure.in
 * </JA>
 *
 * @author Akinobu LEE
 * @date   Thu Feb 10 17:48:32 2005
 *
 * $Revision: 1.6 $
 *
 */

/* Version string */
#define LIBSENT_VERSION "4.1.2_win0"

/* Audio API name */
#define AUDIO_API_NAME "none"

/* Audio API description */
#define AUDIO_API_DESC "NONE!"

/* Description of supported audio file formats */
#define AUDIO_FORMAT_DESC "RAW and WAV only"

/* Description of gzip file reading method */
#define GZIP_READING_DESC "NONE"

/* Define to empty if the keyword does not work on your compiler. */
/* #undef const */

/* Define if you have the ANSI C header files.   */
#define STDC_HEADERS 1

/* Define if your processor stores words with the most significant byte first (like Motorola and SPARC, unlike Intel and VAX).   */
/* #undef WORDS_BIGENDIAN */

/* Define if use microphone input  */


/* Define if use Datlink/Netaudio input  */
/* #undef USE_NETAUDIO */

#define NO_ADINNET  
#define NOWINSOCK2

/* Define if libsndfile support is available */
/* #undef HAVE_LIBSNDFILE */

/* Define for libsndfile support (ver.1)  */
/* #undef HAVE_LIBSNDFILE_VER1 */

/* Define if you have spaudio library  */
/* #undef USE_SPLIB */

/* Define if you use integer word WORD_ID (for vocaburary of over 60k)  */
/* #undef WORDS_INT */

/* Define if you prefer addlog array function */
#define USE_ADDLOG_ARRAY 1

/* Define if you use zcat command for compressed file input */
/* #undef ZCAT */

/* Define if you have socklen definition */
//#define HAVE_SOCKLEN_T 1

/* Define if you have the <sndfile.h> header file.   */
/* #undef HAVE_SNDFILE_H */

/* Define if you have the <unistd.h> header file.   */
#define HAVE_UNISTD_H 1

/* Define if you have the nsl library (-lnsl).   */
/* #undef HAVE_LIBNSL */

/* Define if you have the socket library (-lsocket).   */
/* #undef HAVE_LIBSOCKET */

/* Define if you have zlib library (-lz).  */


/* Define if you have strcasecmp function  */
#define HAVE_STRCASECMP 1

/* Define if you have sleep function  */
#define HAVE_SLEEP 1

/* Define if you have iconv function */
/* #undef HAVE_ICONV */

/* Define if you use libjcode */
/* #undef USE_LIBJCODE */

/* Define if you allow class N-gram  */
#define CLASS_NGRAM 1

/* Define if you want to enable process forking for each adinnet connection */
/* #undef FORK_ADINNET */

/* Define if use sin/cos table for MFCC calculation  */
#define MFCC_SINCOS_TABLE 1

/* Define if <sys/soundcard.h> found */
//#define HAVE_SYS_SOUNDCARD_H 1

/* Define if <machine/soundcard.h> found */
/* #undef HAVE_MACHINE_SOUNDCARD_H */

/* Define if <alsa/asoundlib.h> found */
//#define HAVE_ALSA_ASOUNDLIB_H 1

/* Define if <sys/asoundlib.h> exist and <alsa/asoundlib.h> not exist */
/* #undef HAVE_SYS_ASOUNDLIB_H */

/* Define if <esd.h> exist  */


/* Define if MSD-HMM support is enabled */
/* #undef ENABLE_MSD */
//#define ZCAT "gzip -d -c"

#ifdef _MSC_VER
#define strdup _strdup
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif