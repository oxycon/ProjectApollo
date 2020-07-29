inkling "2.0"
using Number

experiment {
    num_workers: "3",
    env_runners_per_sampler: "2"
}

type GameState {
    P: Number.Float32,
    T: Number.Float32,
    Tw: Number.Float32,
    xA: Number.Float32,
    xB: Number.Float32,
    yA: Number.Float32,
    yB: Number.Float32
}

type Action {
    input_orifice: Number.Float32<Left=1.0, Right=3.5>,
    vent_orifice: Number.Float32<Left=0.8,Right=3.0>,
    blowdown_orifice: Number.Float32<Left=1.0, Right=3.5>,
    real_cycle_time: Number.Float32<Left = 10, Right = 20>,
    vent_time_fract: Number.Float32<Left = 0.6, Right=0.9>
}

type Config {
    episode_length: -1,
    deque_size: 1
}

simulator ApolloSimulator(action: Action, config: Config): GameState {
}

graph (input: GameState): Action {
    concept Balance(input): Action {
        experiment {
            random_seed: "42"
        }
        curriculum {
            source ApolloSimulator
        }
    }
    output Balance
}