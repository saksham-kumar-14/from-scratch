package ratelimiter

import (
	"sync"
	"time"
)

type Limiter struct {
	rate        int
	burst       int
	tokens      int
	lastUpdated time.Time
	mut         sync.Mutex
}

func NewLimiter(rate int, burst int) *Limiter {
	return &Limiter{
		rate:        rate,
		burst:       burst,
		tokens:      burst,
		lastUpdated: time.Now(),
	}
}
