# Copyright 2023-2026 Bruno M. S. Oliveira

from dataclasses import dataclass
from pathlib import Path


@dataclass
class Decay:
    br: float
    daughters: tuple[int, ...]


@dataclass
class Particle:
    pid: int
    width: float
    decays: list[Decay]

    def __str__(self):
        string: str = f"\nParticle {self.pid} ({self.width})"
        for decay in self.decays:
            string += f"\n BR={decay.br:.4f} → {decay.daughters}"

        return string

    def clean_decays(self):
        merged: dict[tuple[int, ...], float] = {}

        for decay in self.decays:
            key = tuple(sorted(abs(pid) for pid in decay.daughters))
            if key not in merged:
                merged[key] = 0.0

            merged[key] += decay.br

        # Rebuild decay list
        self.decays = [Decay(br=br, daughters=key) for key, br in merged.items()]


def hnl_decay_modes(path: Path):
    particles: list[Particle] = []

    current_particle: Particle | None = None

    with open(path) as f:
        for line in f:
            line = line.strip()

            if line.startswith("DECAY"):
                parts = line.split()

                current_particle = Particle(int(parts[1]), float(parts[2]), [])
                particles.append(current_particle)

            elif current_particle and line and not line.startswith("#"):
                parts = line.split()
                if len(parts) >= 4:
                    decay = Decay(float(parts[0]), tuple(map(int, parts[2:-2])))
                    current_particle.decays.append(decay)

    for particle in particles:
        particle.clean_decays()
