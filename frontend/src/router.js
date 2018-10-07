import Vue from 'vue'
import Router from 'vue-router'
import Home from './views/Home.vue'
import Files from './views/Files.vue'

Vue.use(Router)

export default new Router({
  routes: [
    {
      path: '/',
      name: 'home',
      component: Home
    },
    {
      path: '/files',
      name: 'about',
      component: Files
    }
  ]
})
