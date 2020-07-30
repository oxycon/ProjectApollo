inkling "2.0"
using Number

experiment {
    num_workers: "3",
    env_runners_per_sampler: "2"
}

function Reward (gs: GameState) {
    return 1
}

function Terminal (gs: GameState) {
    return false
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
    real_cycle_time: Number.Float32<10 .. 20>,
    vent_time_fract: Number.Float32<0.6 .. 0.9>
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