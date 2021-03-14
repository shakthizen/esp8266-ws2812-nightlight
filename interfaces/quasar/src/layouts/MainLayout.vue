<template>
  <q-layout view="lHh Lpr lFf">
    <q-header elevated>
      <q-toolbar>
        <q-avatar>
          <q-img
            src="icons/favicon-128x128.png"
          />
        </q-avatar>
        <q-toolbar-title>
          SLightString
        </q-toolbar-title>
      </q-toolbar>
    </q-header>
    <q-page-container>
      <router-view />
    </q-page-container>
    <q-ajax-bar ref="bar" position="top" color="orange" size="3px" />
  </q-layout>
</template>

<script>
export default {
  name: "MainLayout",
  data() {
    return {};
  }
};
</script>
