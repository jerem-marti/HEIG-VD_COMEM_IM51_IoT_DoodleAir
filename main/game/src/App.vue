<script setup>
import "./websocket.js"; // Importe le WebSocket pour qu'il soit actif dès le début
import Maze from './Maze.vue';
import { ref, onMounted } from 'vue';

const message = ref("");

onMounted(() => {
    window.addEventListener("button-action", (event) => {
        if (event.detail.button === "color" && event.detail.state === "pressed") {
            window.dispatchEvent(new CustomEvent("boost-ball"));
            message.value = "🏎️ Boost activé !";
        }
        if (event.detail.button === "erase" && event.detail.state === "pressed") {
            window.dispatchEvent(new CustomEvent("reset-game"));
            message.value = "🔄 Jeu réinitialisé !";
        }
    });
});
</script>

<template>
    <Maze />
    <div class="controls">
        <p>{{ message }}</p>
    </div>
</template>

<style>
.controls {
    position: absolute;
    top: 10px;
    left: 50%;
    transform: translateX(-50%);
    background: rgba(0, 0, 0, 0.5);
    padding: 10px;
    border-radius: 10px;
    color: white;
}
</style>
