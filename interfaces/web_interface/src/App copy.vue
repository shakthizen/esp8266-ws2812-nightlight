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
              @change="setEffect()"
            ></v-select>
          </v-col>
        </v-row>
        <v-row>
          <v-col cols="12" sm="12">
            <v-select
              :items="fxAnimations"
              v-model="fxAnimation"
              label="Animation"
              @change="setEffect()"
            ></v-select>
          </v-col>
        </v-row>
      </v-container>
    </v-main>
    <v-bottom-navigation grow v-model="fxValue">
      <v-btn value="-10" @click="setColor(-10)">
        <span>-10</span>

        <v-icon>{{ mdiChevronDoubleLeft }}</v-icon>
      </v-btn>
      <v-btn value="-1" @click="setColor(-1)">
        <span>-1</span>

        <v-icon>{{ mdiChevronLeft }}</v-icon>
      </v-btn>

      <v-btn value="+1" @click="setColor(1)">
        <span>+1</span>

        <v-icon>{{ mdiChevronRight }}</v-icon>
      </v-btn>

      <v-btn value="+10" @click="setColor(10)">
        <span>+10</span>

        <v-icon>{{ mdiChevronDoubleRight }}</v-icon>
      </v-btn>
    </v-bottom-navigation>

    <v-overlay :value="overlay" @click="fetchStatus()">
      <v-progress-circular
        v-if="!connectionFailed"
        indeterminate
        size="64"
      ></v-progress-circular>
      <v-btn
        v-if="connectionFailed"
        outlined
        large
        fab
        color="white"
        @click="fetchStatus()"
        ><v-icon>{{ mdiReload }}</v-icon></v-btn
      >
    </v-overlay>
  </v-app>
</template>

<script>
import {
  mdiChevronDoubleLeft,
  mdiChevronLeft,
  mdiChevronRight,
  mdiChevronDoubleRight,
  mdiReload,
} from "@mdi/js";
export default {
  name: "App",
  data: () => ({
    mdiChevronDoubleLeft,
    mdiChevronLeft,
    mdiChevronRight,
    mdiChevronDoubleRight,
    mdiReload,
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
    fxValue: 0,
    overlay: false,
    connectionFailed: false,
  }),
  computed: {
    fxEffectIndex: function() {
      return this.fxEffects.indexOf(this.fxEffect);
    },
    fxAnimationIndex: function() {
      return this.fxAnimations.indexOf(this.fxAnimation);
    },
  },
  methods: {
    fetchStatus: function() {
      this.overlay = true;
      this.axios
        .get("http://192.168.11.1/api/status")
        .then((response) => {
          let params = response.data.split(",");
          this.fxEffect = this.fxEffects[params[0]];
          this.fxAnimation = this.fxAnimations[params[0]];
          this.fxValue = params[0];
          this.overlay = false;
        })
        .catch(() => {
          this.overlay = true;
          this.connectionFailed = true;
        });
    },
    setEffect: function() {
      this.overlay = true;
      this.axios
        .get(
          `http://192.168.11.1/api/set?fxeffect=${this.fxEffectIndex}&fxanimation=${this.fxAnimationIndex}`
        )
        .then(() => {
          this.overlay = false;
        })
        .catch(() => {
          this.overlay = true;
          this.connectionFailed = true;
        });
    },
    setColor: function(amount) {
      this.overlay = true;
      this.axios
        .get(`http://192.168.11.1/api/set?fxcolor=${amount}`)
        .then(() => {
          this.overlay = false;
        })
        .catch(() => {
          this.overlay = true;
          this.connectionFailed = true;
        });
    },
  },
  created: function() {
    this.fetchStatus();
  },
};
</script>
