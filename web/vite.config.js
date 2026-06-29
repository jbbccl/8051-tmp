import { defineConfig } from 'vite'
import { viteSingleFile } from 'vite-plugin-singlefile'

export default defineConfig({
  root: 'src',
  plugins: [viteSingleFile()],
  server: { port: 5173 },
  build: {
    outDir: '../dist',
    target: 'es2015',
    cssMinify: false,
  },
})
