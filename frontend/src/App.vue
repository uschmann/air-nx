<template>
  <div class="page-container">
    <md-app md-waterfall md-mode="fixed">
      <md-app-toolbar class="md-primary toolbar">
        <md-button class="md-icon-button" @click="showNavigation = true">
          <md-icon>menu</md-icon>
        </md-button>
        <span class="md-title">Air-NX</span>
      </md-app-toolbar>

      <md-app-drawer :md-active.sync="showNavigation" class="drawer">
        <md-toolbar md-elevation="0">
          <span class="md-title">Air-NX</span>
        </md-toolbar>

        <md-list>
          <md-list-item>
            <md-icon>sd_card</md-icon>
            <router-link class="md-list-item-text" to="/files">SD-Card</router-link>
          </md-list-item>
        </md-list>
      </md-app-drawer>

      <md-app-content>
        <div class="wrapper">
          <router-view></router-view>
        </div>
      </md-app-content>
    </md-app>
  </div>
</template>

<script>
export default {
    name: 'App',
    data: () => ({
      showNavigation: false
    })
  }
</script>

<style scoped>
  .wrapper {
    min-height: 100vh;
  }
</style>
