import numpy as np 

class Adam:
    def __init__(self, alpha=0.001, beta1=0.9, beta2=0.999, epsilon=1e-8):
        self.alpha = alpha
        self.beta1 = beta1
        self.beta2 = beta2
        self.epsilon = epsilon
        
        self.m = {}
        self.v = {}
        self.t = 0

    def update(self, params, grads):
        self.t += 1 

        for key in params.keys():
            if key not in self.m:
                self.m[key] = np.zeros_like(params[key])
                self.v[key] = np.zeros_like(params[key])
                
            g = grads[key]
            
            # update biased first moment estimate
            self.m[key] = self.beta1 * self.m[key] + (1 - self.beta1) * g
            
            # update biased second raw moment estimate
            self.v[key] = self.beta2 * self.v[key] + (1 - self.beta2) * (g ** 2)
            
            # compute bias-corrected first moment estimate
            m_hat = self.m[key] / (1 - self.beta1 ** self.t)
            
            # compute bias-corrected second raw moment estimate
            v_hat = self.v[key] / (1 - self.beta2 ** self.t)
            
            # update parameters
            params[key] -= self.alpha * m_hat / (np.sqrt(v_hat) + self.epsilon)
            
        return params

params = {
    "x": np.array([5.0]), 
    "y": np.array([-3.0])
}

optimizer = Adam(alpha=0.1)

epochs = 100
for epoch in range(epochs):
    grads = {
        "x": 2 * params["x"],
        "y": 2 * params["y"]
    }
    
    params = optimizer.update(params, grads)
    
    if epoch % 20 == 0:
        loss = params["x"]**2 + params["y"]**2
        print(f"Epoch {epoch} | x: {params['x'][0]:.4f}, y: {params['y'][0]:.4f} | Loss: {loss[0]:.4f}")

print(f"\nFinal parameters: x = {params['x'][0]:.6f}, y = {params['y'][0]:.6f}")
