/*

   Copyright 2024 Fermi Research Alliance, LLC

   This software was produced under U.S. Government contract DE-AC02-07CH11359
   for Fermi National Accelerator Laboratory (Fermilab), which is operated by
   Fermi Research Alliance, LLC for the U.S. Department of Energy. The U.S.
   Government has rights to use, reproduce, and distribute this software.
   NEITHER THE GOVERNMENT NOR FERMI RESEARCH ALLIANCE, LLC MAKES ANY WARRANTY,
   EXPRESS OR IMPLIED, OR ASSUMES ANY LIABILITY FOR THE USE OF THIS SOFTWARE.
   If software is modified to produce derivative works, such modified software
   should be clearly marked, so as not to confuse it with the version available
   from Fermilab.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR FERMI RESEARCH ALLIANCE, LLC BE LIABLE FOR ANY CLAIM, DAMAGES OR
   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
   IN THE SOFTWARE.

*/

#include <linux/capability.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/capability.h>
#include <sys/prctl.h>
#include <unistd.h>


#define MIN_CAP 1
#define MAX_CAP 100 // This is a semi random constant that you may need to change

void grant_capabilities(cap_value_t caps[], int num_caps) __attribute__((nonnull(1))) __attribute__((flatten));
void grant_capabilities(cap_value_t caps[], int num_caps) {
  // Sanity check num_caps
  if (num_caps < MIN_CAP || num_caps > MAX_CAP) {
    (void)fprintf(stderr, "Error: num_caps %d is out of range. Must be between %d and %d.\n", num_caps, MIN_CAP, MAX_CAP);
    exit(EXIT_FAILURE);
  }

  // Create an empty capability set
  cap_t capabilities = cap_get_proc();
  if (capabilities == NULL) {
    (void)perror("cap_get_proc");
    exit(EXIT_FAILURE);
  }

  // Add specified capabilities to the effective, permitted, and inheritable sets
  if (
      cap_set_flag(capabilities, CAP_EFFECTIVE, num_caps, caps, CAP_SET) == -1
   || cap_set_flag(capabilities, CAP_PERMITTED, num_caps, caps, CAP_SET) == -1
   || cap_set_flag(capabilities, CAP_INHERITABLE, num_caps, caps, CAP_SET) == -1) {
    (void)perror("cap_set_flag");
    (void)cap_free(capabilities);
    exit(EXIT_FAILURE);
  }

  // Apply the new capabilities set
  if (cap_set_proc(capabilities) == -1) {
    (void)perror("cap_set_proc");
    (void)cap_free(capabilities);
    exit(EXIT_FAILURE);
  }

  // Add specified capabilities to the ambient set one at a time
  for (int i=0; i < num_caps; ++i) {
    if (cap_set_ambient(caps[i], CAP_SET) == -1) {
      fprintf(stderr, "Error: cap_set_ambient for capability %d failed: ", caps[i]);
      perror("");
      exit(EXIT_FAILURE);
    }
  }

  // Free the capabilities structure
  if (cap_free(capabilities) == -1) {
    (void)perror("cap_free");
    exit(EXIT_FAILURE);
  }

  // Ensure that the capability is inheritable by children
  if (prctl(PR_SET_KEEPCAPS, 1L) == -1) {
    (void)perror("prctl");
    exit(EXIT_FAILURE);
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    (void)fprintf(stderr, "Usage: %s program [args...]\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  // Define the capabilities you want to grant
  cap_value_t my_caps[] = {CAP_NET_RAW};
  int num_caps = sizeof(my_caps) / sizeof(my_caps[0]);

  (void)grant_capabilities(my_caps, num_caps);

  // Execute the specified program with the given arguments
  execvp(argv[1], &argv[1]);

  // If execvp returns, it means there was an error
  perror("execvp");
  exit(EXIT_FAILURE);
}
