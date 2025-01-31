import cvxpy as cp
import numpy as np

# Define problem parameters
np.random.seed(0)
n_assets = 6  # Stocks, Bonds, Real Estate, Commodities, Crypto, Cash

# Expected returns
mu = np.array([0.12, 0.05, 0.08, 0.10, 0.20, 0.02])  # Annualized expected returns

# Covariance matrix
sigma = np.array([
    [0.10, 0.02, 0.04, 0.03, 0.05, 0.01],
    [0.02, 0.08, 0.03, 0.02, 0.01, 0.02],
    [0.04, 0.03, 0.12, 0.05, 0.04, 0.01],
    [0.03, 0.02, 0.05, 0.09, 0.06, 0.02],
    [0.05, 0.01, 0.04, 0.06, 0.25, 0.01],
    [0.01, 0.02, 0.01, 0.02, 0.01, 0.02]
])

# Portfolio optimization
w = cp.Variable(n_assets)
risk = cp.quad_form(w, sigma)  # Portfolio variance
expected_return = mu @ w       # Portfolio expected return
risk_aversion = 0.0771         # Risk aversion parameter (adjustable)

objective = cp.Maximize(expected_return - risk_aversion * risk)

# Constraints
constraints = [
    cp.sum(w) == 1,                 # Budget constraint
    w[2] >= 0.1,              # Liquidity constraint (≥ 10%)
    w <= 0.4,                        # No asset > 40%
    w >= 0,                          # No short-selling (long-only portfolio)
    risk <= 0.05                    # Maximum allowed risk
]

# Solve
problem = cp.Problem(objective, constraints)
problem.solve()

# Extract results
optimal_weights = w.value
portfolio_return = expected_return.value
portfolio_variance = risk.value
portfolio_stddev = np.sqrt(portfolio_variance)

# Compute Sharpe Ratio using the Excel formula
sharpe_ratio = (portfolio_return - (risk_aversion / 12)) / portfolio_stddev

# Display results
assets = ["Stocks", "Bonds", "Real Estate", "Commodities", "Crypto", "Cash"]
print("\nOptimal Portfolio Weights:")
for asset, weight in zip(assets, optimal_weights):
    print(f"{asset}: {weight:.8f}")

print("\nExpected Portfolio Return:", portfolio_return)
print("Portfolio Risk (Variance):", portfolio_variance)
print("Portfolio Risk (Std Dev):", portfolio_stddev)
print("Sharpe Ratio:", sharpe_ratio)
