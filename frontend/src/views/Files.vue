<template>
  <div @dragover.prevent @drop="onFileDrop">
    <md-toolbar :md-elevation="1">
        <div class="md-toolbar-section-start">
            <md-button @click="back()">
                <md-icon>arrow_back</md-icon>Back
            </md-button>
        </div>
        <div class="md-toolbar-section-end">
            <md-menu md-direction="bottom-start">
                <md-button md-menu-trigger>
                    <md-icon>more_vert</md-icon>
                </md-button>

                <md-menu-content>
                    <md-menu-item @click="makeFolder()">
                        <span>New folder</span>
                        <md-icon>folder</md-icon>
                    </md-menu-item>
                    <md-menu-item @click="openUploadModal">
                        <span>Upload file</span>
                        <md-icon>cloud_upload</md-icon>
                        <input id="fileInput" multiple="multiple" type="file" style="display:none" v-on:change="onFileSelected"/>
                    </md-menu-item>
                </md-menu-content>
            </md-menu>
        </div>
    </md-toolbar>
    <md-subheader><span class="md-title">{{currentDir}}</span></md-subheader>
    <md-progress-spinner v-if="isLoading" md-mode="indeterminate"></md-progress-spinner>
    <div v-else-if="files.length > 0">
        <md-list>
            <md-list-item v-for="file in files" v-bind:key="file.path" @click="fileClicked(file)">
                <md-icon class="md-primary">
                    {{file.is_directory ? 'folder' : 'insert_drive_file'}}
                </md-icon>
                <div class="md-list-item-text">
                    <span>{{file.name}}</span>
                    <span>{{file.is_directory ? 'Folder' : file.extension}}</span>
                </div>

                <md-menu @click.stop md-direction="bottom-start">
                    <md-button md-menu-trigger>
                        <md-icon>more_vert</md-icon>
                    </md-button>

                    <md-menu-content>
                        <md-menu-item @click="deleteFile(file)">
                            <span>Delete</span>
                            <md-icon>delete</md-icon>
                        </md-menu-item>
                    </md-menu-content>
                </md-menu>
            </md-list-item>
        </md-list>
    </div>
    <div v-else>
        <md-empty-state
            md-icon="folder_open"
            md-label="This folder is empty"
            >
    </md-empty-state>
    </div>


<md-dialog-prompt
      :md-active.sync="showNewFolderModal"
      v-model="newFolderName"
      md-title="New Folder?"
      md-input-maxlength="256"
      md-input-placeholder="Name of the folder"
      md-confirm-text="Create folder"
      @md-confirm="createFolder()" />

<md-dialog-confirm
      :md-active.sync="showDeleteModal"
      md-title="Delete?"
      md-content="Do you really want to delete the file?"
      md-confirm-text="Delete"
      md-cancel-text="Cancel"
      @md-cancel="onDeleteCancel"
      @md-confirm="onDeleteConfirm" />

<md-dialog :md-active="filesToUpload.length > 0">
      <md-dialog-title>Upload</md-dialog-title>
      <div class="upload-modal">
        <md-content v-if="filesToUpload.length > 0">
            <h1 class="md-title">{{filesToUpload[0].name}}</h1>
            <p class="md-subheading">{{uploadProgress}}%</p>
            <md-progress-bar class="md-accent" md-mode="determinate" :md-value="uploadProgress"></md-progress-bar>
            <p><strong>{{filesToUpload.length}}</strong> files left.</p>
        </md-content>
      </div>
</md-dialog>

  </div>
</template>

<script>
// @ is an alias to /src
import axios from 'axios';

export default {
  name: 'files',
  components: {
  },
  data: function() {
      return {
          currentDir: '/',
          isLoading: false,
          files: [],
          showNewFolderModal: false,
          newFolderName: '',
          showDeleteModal: false,
          fileToDelete: null,
          showUploadModal: false,
          filesToUpload: [],
          uploadProgress: 0,
      };
  },
  computed: {
      parentDir: function() {
        return this.currentDir.substr(0, this.currentDir.lastIndexOf('/'));
      }
  },
  methods: {
      load: function(path) {
          if(!path) {
              path = '/';
          }
          this.isLoading = true;
          axios.get(`api/file?dir=${path}`)
          .then(result => {
            this.files = result.data;
            this.currentDir = path;
            this.isLoading = false;
          });
      },
      fileClicked: function(file) {
          if(file.is_directory) {
              this.load(file.path);
          }
          else {
              window.open('/download' + file.path, '_blank');
          }
      },
      back: function() {
          this.load(this.parentDir);
      },
      makeFolder: function() {
          this.newFolderName = '';
          this.showNewFolderModal = true;
      },
      createFolder: function() {
          let parent = this.currentDir;
          if(!parent.endsWith('/')) {
            parent += '/';
          }
          axios.get(`api/directory?path=${parent}${this.newFolderName}`)
            .then(result => {
                this.files.push(result.data);
            });
      },
      deleteFile: function(file) {
          this.fileToDelete = file;
          this.showDeleteModal = true;
      },
      onDeleteCancel: function() {
          this.fileToDelete = null;
          this.showDeleteModal = false;
      },
      onDeleteConfirm: function() {
           axios.delete(`api/file?path=${this.fileToDelete.path}`)
            .then(result => {
                this.files = this.files.filter(file => file.path != result.data.path);
            });
      },
      upload: function() {
          const self = this;
          self.uploadProgress = 0;

          const config = {
                onUploadProgress: function(progressEvent) {
                    var percentCompleted = Math.round( (progressEvent.loaded * 100) / progressEvent.total )
                    self.uploadProgress = percentCompleted;
                }
            }

            let data = new FormData();
            data.append('file', this.filesToUpload[0]);

            let path = this.currentDir;
            if(!path.endsWith('/')) {
                path += '/';
            }
            path += this.filesToUpload[0].name;

             axios.put(`/api/upload?path=${path}`, data, config)
            .then(res => {
                self.files.push(res.data);   
                self.filesToUpload.shift();   
                if(self.filesToUpload.length > 0) {
                    self.upload();
                }
            })
            .catch(err => console.log(err));
      },
      onFileDrop: function(e) {
            e.stopPropagation();
            e.preventDefault();
            var files = e.dataTransfer.files;
            console.log(files);

            for(var i = 0; i < files.length; i++) {
                this.filesToUpload.push(files[i]);
            }
            this.upload();
      },
      onFileSelected: function(e, target) {
        var files = e.target.files;
        for(var i = 0; i < files.length; i++) {
                this.filesToUpload.push(files[i]);
            }
        this.upload();
      },
      openUploadModal: function() {
          document.getElementById('fileInput').click();
      }
  },
  mounted: function() {
      this.load(this.currentDir);
  }
}
</script>
 
<style scoped>
.upload-modal {
    padding: 40px;
    min-width: 400px;
}
</style>