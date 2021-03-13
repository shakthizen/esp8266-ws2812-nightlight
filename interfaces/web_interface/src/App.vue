<template>
  <v-app>
    <v-app-bar app flat dense color="primary text-center" dark>
      <v-toolbar-title>SS LIGHTSTRING</v-toolbar-title>
    </v-app-bar>

    <v-main>
      <v-container>
        <v-row>
          <v-col cols="12" sm="12">
            <v-select
              :items="fxEffects"
              v-model="fxEffect"
              label="Effect"
            ></v-select>
          </v-col>
        </v-row>
        <v-row>
          <v-col cols="12" sm="12">
            <v-select
              :items="fxAnimations"
              v-model="fxAnimation"
              label="Animation"
            ></v-select>
          </v-col>
        </v-row>
      </v-container>
    </v-main>
    <v-bottom-navigation v-model="value">
      <v-btn value="-10">
        <span>-10</span>

        <v-icon>{{ mdiChevronDoubleLeft }}</v-icon>
      </v-btn>
      <v-btn value="-1">
        <span>-1</span>

        <v-icon>{{ mdiChevronLeft }}</v-icon>
      </v-btn>

      <v-btn value="+1">
        <span>+1</span>

        <v-icon>{{ mdiChevronRight }}</v-icon>
      </v-btn>

      <v-btn value="+10">
        <span>+10</span>

        <v-icon>{{ mdiChevronDoubleRight }}</v-icon>
      </v-btn>
    </v-bottom-navigation>
  </v-app>
</template>

<script>
import {
  mdiChevronDoubleLeft,
  mdiChevronLeft,
  mdiChevronRight,
  mdiChevronDoubleRight,
} from "@mdi/js";
export default {
  name: "App",
  data: () => ({
    mdiChevronDoubleLeft,
    mdiChevronLeft,
    mdiChevronRight,
    mdiChevronDoubleRight,
    fxEffects: [
      "White",
      "Color",
      "Gradient",
      "Gradient (Forward Animation)",
      "Gradient (Backward Animation)",
      "Rainbow",
      "Rainbow (Forward Animation)",
      "Rainbow (Backward Animation)",
    ],
    fxEffect: "",
    fxAnimations: [
      "None",
      "Glitter",
      "Breathing",
      "Wipe Forward (Slow)",
      "Wipe Backward (Slow)",
      "Wipe Forward (Fast)",
      "Wipe Backward (Fast)",
      "Chaser Forward",
      "Chaser Backward",
      "Meteor Forward",
      "Meteor Backward",
      "Sinelon",
      "Fire",
      "Fire Colored",
    ],
    fxAnimation: "",
    value: 0
  }),
  computed: {
    fxEffectIndex: function() {
      return this.fxEffects.indexOf(this.fxEffect);
    },
  },
};
</script>
