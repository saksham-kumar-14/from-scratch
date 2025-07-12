package ratelimiter

import "net/http"

type getLimiterFunc func(string) *Limiter

func RateLimitMiddleware(next http.Handler, getLimiterForIP getLimiterFunc) http.Handler {
	return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		ip := r.RemoteAddr

		limiter := getLimiterForIP(ip)
		if !limiter.Allow() {
			http.Error(w, "Rate limit exceeded", http.StatusTooManyRequests)
			return
		}

		next.ServeHTTP(w, r)
	})
}
