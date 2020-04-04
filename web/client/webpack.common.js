const MonacoWebpackPlugin = require('monaco-editor-webpack-plugin');
const CleanWebpackPlugin = require('clean-webpack-plugin');
const path = require('path');

const outputPath = path.resolve(__dirname, 'build');

module.exports = {
  mode: 'none',
  entry: {
    app: './src/index.ts'
  },
  module: {
    rules: [
      {
        test: /\.tsx?$/,
        use: 'ts-loader',
        exclude: /node_modules/
      },
      {
        test: /\.css$/,
        use: [ 'style-loader', 'css-loader' ]
      }
    ]
  },
  plugins: [
    new MonacoWebpackPlugin({
      "languages": [],
      "features": [
        'bracketMatching', 'caretOperations', 'clipboard', 'codelens', 'colorDetector', 'comment', 'contextmenu',
        'coreCommands', 'cursorUndo', 'find', 'folding', 'format', 'gotoLine', 'hover', 'inPlaceReplace', 'inspectTokens', 'linesOperations', 'links',
        'parameterHints', 'rename', 'smartSelect', 'snippets', 'suggest', 'wordHighlighter', 'wordOperations'
      ]
    }),
    new CleanWebpackPlugin(outputPath)
  ],
  resolve: {
    extensions: [ '.tsx', '.ts', '.js', '.css' ]
  },
  output: {
    filename: 'bundle.js',
    path: outputPath,
    libraryTarget: 'var',
    library: 'SCsEditor'
  },
  node: {
    fs: "empty"
  }
};