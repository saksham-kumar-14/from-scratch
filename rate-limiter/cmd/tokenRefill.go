package ratelimiter

import "time"

func (l *Limiter) Allow() bool {
	l.mut.Lock()
	defer l.mut.Unlock()

	now := time.Now()
	elapsed := now.Sub(l.lastUpdated)

	newToken := int(elapsed.Seconds() * float64(l.rate))
	if newToken > 0 {
		l.tokens = min(l.burst, l.tokens+newToken)
		l.lastUpdated = time.Now()
	}

	if l.tokens > 0 {
		l.tokens--
		return true
	}

	return false
}

func min(a int, b int) int {
	if a < b {
		return a
	}
	return b
}
