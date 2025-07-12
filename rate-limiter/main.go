package main

import (
	"fmt"
	"net/http"
	ratelimiter "ratelimiter/cmd"
	"sync"
)

var (
	limiters = make(map[string]*ratelimiter.Limiter)
	mu       sync.Mutex
)

func getLimiterForIP(ip string) *ratelimiter.Limiter {
	mu.Lock()
	defer mu.Unlock()

	limiter, exists := limiters[ip]
	if !exists {
		limiter = ratelimiter.NewLimiter(5, 10)
		limiters[ip] = limiter
	}
	return limiter
}

func Handler(w http.ResponseWriter, r *http.Request) {
	fmt.Println("Passed!")
}

func main() {
	mux := http.NewServeMux()
	mux.Handle("/", ratelimiter.RateLimitMiddleware(http.HandlerFunc(Handler), getLimiterForIP))

	fmt.Println("Server started on http://localhost:8080")
	http.ListenAndServe(":8080", mux)
}
