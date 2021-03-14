<template>
  <q-page class="q-pa-md">
    <div class="row q-pt-md">
      <div class="col">
        <q-select
          v-model="fxEffect"
          :options="fxEffects"
          label="Effect"
          outlined
          :dropdown-icon="mdiMenuDown"
          @input="setEffect()"
        />
      </div>
    </div>
    <div class="row q-pt-md">
      <div class="col">
        <q-select
          v-model="fxAnimation"
          :options="fxAnimations"
          label="Animation"
          outlined
          :dropdown-icon="mdiMenuDown"
          @input="setEffect()"
        />
      </div>
    </div>
    <div class="row q-pt-md">
      <div class="col flex flex-center q-gutter-md">
        <q-btn
          stack
          round
          size="lg"
          outline
          color="blue"
          :icon="mdiChevronDoubleLeft"
          @click="setColor(-10)"
        />
        <q-btn
          stack
          round
          size="lg"
          outline
          color="blue"
          :icon="mdiChevronLeft"
          @click="setColor(-1)"
        />
        <q-btn
          stack
          round
          size="lg"
          outline
          color="blue"
          :icon="mdiChevronRight"
          @click="setColor(+1)"
        />
        <q-btn
          stack
          round
          size="lg"
          outline
          color="blue"
          :icon="mdiChevronDoubleRight"
          @click="setColor(+10)"
        />
      </div>
    </div>
    <q-inner-loading :showing="connectionFailed" dark>
      <q-spinner-gears size="50px" color="white" />
    </q-inner-loading>
    <q-page-sticky position="bottom-right" :offset="[18, 18]">
      <q-btn fab :icon="mdiReload" color="blue" @click="fetchStatus()" />
    </q-page-sticky>
  </q-page>
</template>

<script>
import {
  mdiChevronDoubleLeft,
  mdiChevronLeft,
  mdiChevronRight,
  mdiChevronDoubleRight,
  mdiReload,
  mdiMenuDown
} from "@quasar/extras/mdi-v5";
export default {
  name: "Index",
  data: () => ({
    mdiChevronDoubleLeft,
    mdiChevronLeft,
    mdiChevronRight,
    mdiChevronDoubleRight,
    mdiReload,
    mdiMenuDown,
    fxEffects: [
      "White",
      "Color",
      "Gradient",
      "Gradient (Forward Animation)",
      "Gradient (Backward Animation)",
      "Rainbow",
      "Rainbow (Forward Animation)",
      "Rainbow (Backward Animation)"
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
      "Random Fade",
      "Fire",
      "Fire Colored"
    ],
    fxAnimation: "",
    fxValue: 0,
    overlay: false,
    connectionFailed: false
  }),
  computed: {
    fxEffectIndex: function() {
      return this.fxEffects.indexOf(this.fxEffect);
    },
    fxAnimationIndex: function() {
      return this.fxAnimations.indexOf(this.fxAnimation);
    }
  },
  methods: {
    fetchStatus: function() {
      this.overlay = true;
      this.$axios
        .get("http://192.168.11.1/api/status")
        .then(response => {
          let params = response.data.split(",");
          this.fxEffect = this.fxEffects[params[0]];
          this.fxAnimation = this.fxAnimations[params[1]];
          this.fxValue = params[2];
          this.overlay = false;
        })
        .catch(() => {
          this.overlay = true;
          this.connectionFailed = true;
        });
    },
    setEffect: function() {
      this.overlay = true;
      this.$axios
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
      this.$axios
        .get(`http://192.168.11.1/api/set?fxcolor=${amount}`)
        .then(() => {
          this.overlay = false;
        })
        .catch(() => {
          this.overlay = true;
          this.connectionFailed = true;
        });
    }
  },
  created: function() {
    this.fetchStatus();
  }
};
</script>
