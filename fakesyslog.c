#include <stdarg.h>
#include <sys/socket.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <syslog.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <fcntl.h>

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static char log_ident[32];
static int log_opt;
static int log_facility = LOG_USER;
static int log_mask = 0xff;

#define LOCK(l) pthread_mutex_lock(&l)
#define UNLOCK(l) pthread_mutex_unlock(&l)

int setlogmask(int maskpri)
{
	LOCK(lock);
	int ret = log_mask;
	if (maskpri) log_mask = maskpri;
	UNLOCK(lock);
	return ret;
}

void closelog(void)
{
}


void openlog(const char *ident, int opt, int facility)
{
	int cs;
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cs);
	LOCK(lock);

	if (ident) {
		size_t n = strnlen(ident, sizeof log_ident - 1);
		memcpy(log_ident, ident, n);
		log_ident[n] = 0;
	} else {
		log_ident[0] = 0;
	}
	log_opt = opt;
	log_facility = facility;

	UNLOCK(lock);
	pthread_setcancelstate(cs, 0);
}

static void _vsyslog(int priority, const char *message, va_list ap)
{
	char timebuf[16];
	time_t now;
	struct tm tm;
	char buf[1024];
	int errno_save = errno;
	int pid;
	int l, l2;
	int hlen;

	if (!(priority & LOG_FACMASK)) priority |= log_facility;

	now = time(NULL);
	gmtime_r(&now, &tm);
	strftime(timebuf, sizeof timebuf, "%b %e %T", &tm);

	pid = (log_opt & LOG_PID) ? getpid() : 0;
	l = snprintf(buf, sizeof buf, "<%d>%s %n%s%s%.0d%s: ",
		priority, timebuf, &hlen, log_ident, "["+!pid, pid, "]"+!pid);
	errno = errno_save;
	l2 = vsnprintf(buf+l, sizeof buf - l, message, ap);
	if (l2 >= 0) {
		if (l2 >= sizeof buf - l) l = sizeof buf - 1;
		else l += l2;
		if (buf[l-1] != '\n') buf[l++] = '\n';
		dprintf(STDERR_FILENO, "%.*s", l-hlen, buf+hlen);
	}
}

void vsyslog(int priority, const char *message, va_list ap)
{
	int cs;
	if (!(log_mask & LOG_MASK(priority&7)) || (priority&~0x3ff)) return;
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cs);
	LOCK(lock);
	_vsyslog(priority, message, ap);
	UNLOCK(lock);
	pthread_setcancelstate(cs, 0);
}

void syslog(int priority, const char *message, ...)
{
	va_list ap;
	va_start(ap, message);
	vsyslog(priority, message, ap);
	va_end(ap);
}
