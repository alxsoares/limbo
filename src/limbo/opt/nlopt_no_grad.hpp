#ifndef LIMBO_OPT_NLOPT_NO_GRAD_HPP
#define LIMBO_OPT_NLOPT_NO_GRAD_HPP

#ifndef USE_NLOPT
#warning No NLOpt
#else
#include <Eigen/Core>

#include <vector>

#include <nlopt.hpp>

#include <limbo/tools/macros.hpp>

namespace limbo {
    namespace defaults {
        struct opt_nloptnograd {
            BO_PARAM(int, iterations, 500);
        };
    }
    namespace opt {
        template <typename Params, nlopt::algorithm Algorithm>
        struct NLOptNoGrad {
        public:
            template <typename F>
            Eigen::VectorXd operator()(const F& f, bool bounded) const
            {
                // Assert that the algorithm is non-gradient
                // TO-DO: Add support for MLSL (Multi-Level Single-Linkage)
                // TO-DO: Add better support for ISRES (Improved Stochastic Ranking Evolution Strategy)
                static_assert(Algorithm == nlopt::LN_COBYLA || Algorithm == nlopt::LN_BOBYQA || Algorithm == nlopt::LN_NEWUOA || Algorithm == nlopt::LN_NEWUOA_BOUND || Algorithm == nlopt::LN_PRAXIS || Algorithm == nlopt::LN_NELDERMEAD || Algorithm == nlopt::LN_SBPLX || Algorithm == nlopt::GN_DIRECT || Algorithm == nlopt::GN_DIRECT_L || Algorithm == nlopt::GN_DIRECT_L_RAND || Algorithm == nlopt::GN_DIRECT_NOSCAL || Algorithm == nlopt::GN_DIRECT_L_NOSCAL || Algorithm == nlopt::GN_DIRECT_L_RAND_NOSCAL || Algorithm == nlopt::GN_ORIG_DIRECT || Algorithm == nlopt::GN_ORIG_DIRECT_L || Algorithm == nlopt::GN_CRS2_LM || Algorithm == nlopt::GD_STOGO || Algorithm == nlopt::GD_STOGO_RAND || Algorithm == nlopt::GN_ISRES || Algorithm == nlopt::GN_ESCH, "NLOptNoGrad accepts gradient free nlopt algorithms only");

                nlopt::opt opt(Algorithm, f.param_size());

                opt.set_max_objective(nlopt_func<F>, (void*)&f);

                std::vector<double> x(f.init().size());
                Eigen::VectorXd::Map(&x[0], f.init().size()) = f.init();

                opt.set_maxeval(Params::opt_nloptnograd::iterations());

                if (bounded) {
                    opt.set_lower_bounds(std::vector<double>(f.param_size(), 0));
                    opt.set_upper_bounds(std::vector<double>(f.param_size(), 1));
                }

                double max;

                opt.optimize(x, max);

                return Eigen::VectorXd::Map(x.data(), x.size());
            }

        protected:
            template <typename F>
            static double nlopt_func(const std::vector<double>& x, std::vector<double>& grad, void* my_func_data)
            {
                F* f = (F*)(my_func_data);
                Eigen::VectorXd params = Eigen::VectorXd::Map(x.data(), x.size());
                double v = f->utility(params);
                return v;
            }
        };
    }
}

#endif
#endif
