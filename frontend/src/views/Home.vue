<template>
  <div class="home">
    <div class="md-layout">
      <div class="md-layout-item">
        <md-card>
          <md-card-header>
            <md-card-header-text>
              <div class="md-title">Welcome to Air-NX</div>
              <div class="md-subhead">Version: 1.0</div>
            </md-card-header-text>
          </md-card-header>
          <md-card-content>
            I hope you enjoy to use Air-NX.<br>
            Please feel free to report any issues you may have<br>
            on github.
          </md-card-content>
          <md-card-actions>
            <md-button>
              <md-icon>link</md-icon>
              <a href="https://github.com/uschmann/air-nx" target="_blank">Visit Air-NX on github</a>
            </md-button>
          </md-card-actions>
        </md-card>
      </div>
      <div class="md-layout-item">

      <md-card>
        <md-card-header>
          <md-card-header-text>
            <div class="md-title">Manage SD-Card</div>
          </md-card-header-text>
        </md-card-header>
        <md-card-content>
          Browse files on your SD-Card. Easilly upload files to the SD-Card by dragging them into the browser window.
        </md-card-content>
        <md-card-actions>
          <md-button>
            <router-link class="md-list-item-text" to="/files">Manage SD-Card now</router-link>
          </md-button>
        </md-card-actions>
      </md-card>

      </div>
    </div>
  </div>
</template>

<script>
// @ is an alias to /src

export default {
  name: 'home',
  components: {
    
  }
}
</script>
