const { override,disableEsLint,addWebpackAlias,addLessLoader,addDecoratorsLegacy } = require('customize-cra');
const CompressionWebpackPlugin = require('compression-webpack-plugin');
const { BundleAnalyzerPlugin } = require('webpack-bundle-analyzer')

//配置开发模式和打包模式
const addCustomize = () => config => {
  // 打包模式
  if(process.env.NODE_ENV === 'production') {
    config.devtool = false; //去除map文件
     // 添加js打包gzip配置
    addPostcssPlugins([require("postcss-px2rem-exclude")({ remUnit: 37.5, exclude: /node_modules/i })])
    disableEsLint()
    addWebpackAlias({
        ["@/"]: path.resolve(__dirname, "src/")
      })
    addWebpackPlugin(
          new BundleAnalyzerPlugin({analyzerMode: 'static'})
    )
    addDecoratorsLegacy()
    config.plugins = [...config.plugins, new CompressionWebpackPlugin({
      test: /.js$|.css$/, // 压缩js与css
      threshold: 1024, // 只处理比这个值大的资源，单位字节
    })]
  }
   else if(process.env.NODE_ENV === 'development'){
  }
  return config
}

module.exports = override(
 );
