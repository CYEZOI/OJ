const Path = require("path");
const TerserPlugin = require("terser-webpack-plugin");
const CopyWebpackPlugin = require('copy-webpack-plugin');

module.exports = {
    mode: "production",
    entry: Path.resolve(__dirname, "index.tsx"),
    output: {
        filename: "index.js",
        path: Path.resolve(__dirname, "..", "Build", "Static"),
    },
    devtool: "source-map",
    resolve: {
        extensions: [".ts", ".tsx", ".js"],
    },
    module: {
        rules: [
            {
                test: /\.tsx?$/,
                use: "ts-loader",
                exclude: /node_modules/,
            },
        ],
    },
    optimization: {
        minimizer: [new TerserPlugin({
            extractComments: false,
        })],
    },
    performance: {
        hints: false,
    },
    plugins: [
        new CopyWebpackPlugin({
            patterns: [
                {
                    from: Path.resolve(__dirname, "Static"),
                    to: Path.resolve(__dirname, "..", "Build", "Static")
                },
            ],
        }),
    ],
};
